
#include "acoustics_interface_node.hpp"

#include "acoustics_interface_driver.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"

#include <chrono>
#include <string>
#include <utility>
#include <vector>

AcousticsInterfaceNode::AcousticsInterfaceNode() : Node("acoustics_interface") {
    // Create publishers
    hydrophone1_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/hydrophone1", 5);
    hydrophone2_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/hydrophone2", 5);
    hydrophone3_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/hydrophone3", 5);
    hydrophone4_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/hydrophone4", 5);
    hydrophone5_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/hydrophone5", 5);

    filter_response_pub_ =
        this->create_publisher<std_msgs::msg::Int32MultiArray>(
            "/acoustics/filter_response", 5);
    fft_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/fft", 5);
    peak_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
        "/acoustics/peaks", 5);
    tdoa_pub_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
        "/acoustics/time_difference_of_arrival", 5);
    position_pub_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
        "/acoustics/position", 5);

    // Declare parameter: data logging rate (default 1.0 sample per second)
    this->declare_parameter<double>("acoustics.data_logging_rate", 1.0);
    double data_logging_rate =
        this->get_parameter("acoustics.data_logging_rate").as_double();
    auto timer_period = std::chrono::duration<double>(1.0 / data_logging_rate);

    // Create timers:
    // Data update timer: calls data_update() frequently (e.g. every 1ms)
    data_update_timer_ = this->create_wall_timer(
        std::chrono::milliseconds(1),
        std::bind(&AcousticsInterfaceNode::data_update, this));
    // Data publisher timer: publishes data according to the logging rate
    data_publisher_timer_ = this->create_wall_timer(
        std::chrono::duration_cast<std::chrono::milliseconds>(timer_period),
        std::bind(&AcousticsInterfaceNode::data_publisher, this));

    // 1) Declare the parameter as a vector<int64_t> with a default size‐20
    // array of zeros:
    this->declare_parameter<std::vector<int64_t>>(
        "acoustics.frequencies_of_interest", std::vector<int64_t>(20, 0));

    // 2) When you get it back, store it in a vector<int64_t>:
    std::vector<int64_t> freq_params =
        this->get_parameter("acoustics.frequencies_of_interest")
            .as_integer_array();

    FrequencyInterest frequencyInterest[NUM_FREQ_INTERESTS];
    // Parse the parameters into a vector of frequency-variance pairs.
    for (size_t i = 0; i < freq_params.size(); i += 2) {
        frequencyInterest[i].frequency = freq_params[2 * i];
        frequencyInterest[i].variance = freq_params[2 * i + 1];
    }

    init_communication(&comm, frequencyInterest, 10);
}

void AcousticsInterfaceNode::data_update() {
    fetch_data(&comm);
}

void AcousticsInterfaceNode::data_publisher() {
    std_msgs::msg::Int32MultiArray int_msg;
    std_msgs::msg::Float32MultiArray float_msg;

    // 1) Publish hydrophone1 (int16_t → int32_t)
    int_msg.data.clear();
    int_msg.data.reserve(RAW_HYDROPHONE_SIZE);
    for (size_t i = 0; i < RAW_HYDROPHONE_SIZE; ++i) {
        int_msg.data.push_back(
            static_cast<int32_t>(samples_raw_hydrophone1[i]));
    }
    hydrophone1_pub_->publish(int_msg);

    // 2) Publish hydrophone2
    int_msg.data.clear();
    int_msg.data.reserve(RAW_HYDROPHONE_SIZE);
    for (size_t i = 0; i < RAW_HYDROPHONE_SIZE; ++i) {
        int_msg.data.push_back(
            static_cast<int32_t>(samples_raw_hydrophone2[i]));
    }
    hydrophone2_pub_->publish(int_msg);

    // 3) Publish hydrophone3
    int_msg.data.clear();
    int_msg.data.reserve(RAW_HYDROPHONE_SIZE);
    for (size_t i = 0; i < RAW_HYDROPHONE_SIZE; ++i) {
        int_msg.data.push_back(
            static_cast<int32_t>(samples_raw_hydrophone3[i]));
    }
    hydrophone3_pub_->publish(int_msg);

    // 4) Publish hydrophone4
    int_msg.data.clear();
    int_msg.data.reserve(RAW_HYDROPHONE_SIZE);
    for (size_t i = 0; i < RAW_HYDROPHONE_SIZE; ++i) {
        int_msg.data.push_back(
            static_cast<int32_t>(samples_raw_hydrophone4[i]));
    }
    hydrophone4_pub_->publish(int_msg);

    // 5) Publish hydrophone5
    int_msg.data.clear();
    int_msg.data.reserve(RAW_HYDROPHONE_SIZE);
    for (size_t i = 0; i < RAW_HYDROPHONE_SIZE; ++i) {
        int_msg.data.push_back(
            static_cast<int32_t>(samples_raw_hydrophone5[i]));
    }
    hydrophone5_pub_->publish(int_msg);

    // 6) Publish filtered samples array (size = SAMPLE_LENGTH)
    int_msg.data.clear();
    int_msg.data.reserve(SAMPLE_LENGTH);
    for (size_t i = 0; i < SAMPLE_LENGTH; ++i) {
        int_msg.data.push_back(static_cast<int32_t>(samples_filtered[i]));
    }
    filter_response_pub_->publish(int_msg);

    // 7) Publish FFT magnitudes (size = SAMPLE_LENGTH)
    int_msg.data.clear();
    int_msg.data.reserve(SAMPLE_LENGTH);
    for (size_t i = 0; i < SAMPLE_LENGTH; ++i) {
        int_msg.data.push_back(static_cast<int32_t>(fft_magnified[i]));
    }
    fft_pub_->publish(int_msg);

    // 8) Publish time_diff (float[5])
    float_msg.data.clear();
    float_msg.data.reserve(5);
    for (size_t i = 0; i < 5; ++i) {
        float_msg.data.push_back(time_diff[i]);
    }
    tdoa_pub_->publish(float_msg);

    // 9) Publish position (float[4])
    float_msg.data.clear();
    float_msg.data.reserve(4);
    for (size_t i = 0; i < 4; ++i) {
        float_msg.data.push_back(position[i]);
    }
    position_pub_->publish(float_msg);
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AcousticsInterfaceNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
