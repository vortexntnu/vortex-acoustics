from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='acoustics_interface',
            executable='acoustics_interface_node.py'
        )
    ])