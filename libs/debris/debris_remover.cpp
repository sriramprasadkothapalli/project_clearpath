// MIT License
//
// Copyright (c) 2024 Tathya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * @file debris_remover.cpp
 * @brief Implementation of debris remover 
 */

#include "debris_remover.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <gazebo_msgs/srv/detail/delete_entity__struct.hpp>
#include <rclcpp/executors.hpp>
#include <rclcpp/future_return_code.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/utilities.hpp>

using std::chrono_literals::operator""s;
using std::placeholders::_1;

/**
 * @brief Constructor for the DebrisRemover node.
 * Initializes the `/delete_entity` client and debris removal node.
 */
DebrisRemover::DebrisRemover() : rclcpp::Node("debris_remover") {
  unspawn_client =
      create_client<gazebo_msgs::srv::DeleteEntity>("/delete_entity");

  remove_debris_node = rclcpp::Node::make_shared("debris_remover");
}

/**
 * @brief Removes debris from the environment by interacting with the `/delete_entity` service.
 * @param object The name of the debris object to be removed.
 * @return True if the debris was successfully removed, false otherwise.
 */
bool DebrisRemover::remove_debris(std::string object) {
  while (!unspawn_client->wait_for_service(5s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(), "Service Call Failed");
      return false;
    }
    RCLCPP_INFO(this->get_logger(), "Service Not Available");
    return false;
  }

  auto request = std::make_shared<gazebo_msgs::srv::DeleteEntity::Request>();
  request->name = object;

  auto srv_response = unspawn_client->async_send_request(request);
  auto ret =
      rclcpp::spin_until_future_complete(remove_debris_node, srv_response, 3s);

  return ret == rclcpp::FutureReturnCode::SUCCESS;
}
