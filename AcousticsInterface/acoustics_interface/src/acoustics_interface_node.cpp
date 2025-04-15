
#include "acoustics_interface_node.hpp"

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

  filter_response_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>(
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
      1ms, std::bind(&AcousticsInterfaceNode::data_update, this));
  // Data publisher timer: publishes data according to the logging rate
  data_publisher_timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::milliseconds>(timer_period),
      std::bind(&AcousticsInterfaceNode::data_publisher, this));

  // Declare parameter for frequencies of interest with a default list of 20
  // zeros.
  this->declare_parameter<std::vector<int>>("acoustics.frequencies_of_interest",
                                            std::vector<int>(20, 0));
  std::vector<int> freq_params =
      this->get_parameter("acoustics.frequencies_of_interest")
          .as_integer_array();

  // Parse the parameters into a vector of frequency-variance pairs.
  std::vector<std::pair<int, int>> frequencies_of_interest;
  for (size_t i = 0; i < freq_params.size(); i += 2) {
    frequencies_of_interest.push_back({freq_params[i], freq_params[i + 1]});
  }

  // Initialize communication with TeensyCommunicationUDP. This is expected to
  // be a static method.
  this->get_logger()->info("Initializing communication with Acoustics");
  this->get_logger()->info("Acoustics PCB MCU IP: 10.0.0.111");
  this->get_logger()->info("Trying to connect...");
  init_communication(frequencies_of_interest);
  this->get_logger()->info("Successfully connected to Acoustics PCB MCU :D");
}

void AcousticsInterfaceNode::data_update() {
  fetch_data();
}

void AcousticsInterfaceNode::data_publisher() {
  std_msgs::msg::Int32MultiArray int_msg;
  std_msgs::msg::Float32MultiArray float_msg;

  int_msg.data = acoustics_data_int["HYDROPHONE_1"];
  hydrophone1_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["HYDROPHONE_2"];
  hydrophone2_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["HYDROPHONE_3"];
  hydrophone3_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["HYDROPHONE_4"];
  hydrophone4_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["HYDROPHONE_5"];
  hydrophone5_pub_->publish(int_msg);

  int_msg.data = acoustics_data_int["SAMPLES_FILTERED"];
  filter_response_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["FFT"];
  fft_pub_->publish(int_msg);
  int_msg.data = acoustics_data_int["PEAK"];
  peak_pub_->publish(int_msg);

  float_msg.data = acoustics_data_float["TDOA"];
  tdoa_pub_->publish(float_msg);
  float_msg.data = acoustics_data_float["LOCATION"];
  position_pub_->publish(float_msg);
}


int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<AcousticsInterfaceNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
