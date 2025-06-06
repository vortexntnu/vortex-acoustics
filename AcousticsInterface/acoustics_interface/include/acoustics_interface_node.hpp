
#ifndef ACOUSTICS_INTERFACE_NODE_HPP
#define ACOUSTICS_INTERFACE_NODE_HPP

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "acoustics_interface_driver.h"


class AcousticsInterfaceNode : public rclcpp::Node {
public:
  AcousticsInterfaceNode();
  ~AcousticsInterfaceNode() = default;

private:
  void data_update();
  void data_publisher();
  TeensyCommunicationUDP comm;

  // Publishers for acoustics topics
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hydrophone1_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hydrophone2_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hydrophone3_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hydrophone4_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hydrophone5_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr filter_response_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr fft_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr peak_pub_;
  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr tdoa_pub_;
  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr position_pub_;

  // Timers
  rclcpp::TimerBase::SharedPtr data_update_timer_;
  rclcpp::TimerBase::SharedPtr data_publisher_timer_;
};

#endif  // ACOUSTICS_INTERFACE_NODE_HPP
