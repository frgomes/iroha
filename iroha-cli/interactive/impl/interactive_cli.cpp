/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interactive/interactive_cli.hpp"

#include <ciso646>
#include <utility>

namespace iroha_cli {
  namespace interactive {

    void InteractiveCli::assign_main_handlers() {
      addCliCommand(menu_points_,
                    main_handler_map_,
                    TX_CODE,
                    "New transaction",
                    &InteractiveCli::startTx);
      addCliCommand(menu_points_,
                    main_handler_map_,
                    QRY_CODE,
                    "New query",
                    &InteractiveCli::startQuery);
      addCliCommand(menu_points_,
                    main_handler_map_,
                    ST_CODE,
                    "New transaction status request",
                    &InteractiveCli::startTxStatusRequest);
    }

    InteractiveCli::InteractiveCli(
        const std::string &account_name,
        const std::string &default_peer_ip,
        int default_port,
        uint64_t qry_counter,
        const std::shared_ptr<iroha::model::ModelCryptoProvider> &provider)
        : creator_(account_name),
          tx_cli_(creator_, default_peer_ip, default_port, provider),
          query_cli_(
              creator_, default_peer_ip, default_port, qry_counter, provider),
          statusCli_(default_peer_ip, default_port) {
      assign_main_handlers();
    }

    void InteractiveCli::parseMain(std::string line) {
      auto raw_command = parser::parseFirstCommand(std::move(line));
      if (not raw_command) {
        handleEmptyCommand();
        return;
      }
      auto command_name = *raw_command;

      auto val = findInHandlerMap(command_name, main_handler_map_);
      if (val) {
        (this->*val.value())();
      }
    }

    void InteractiveCli::startQuery() {
      query_cli_.run();
    }

    void InteractiveCli::startTx() {
      tx_cli_.run();
    }

    void InteractiveCli::startTxStatusRequest() {
      statusCli_.run();
    }

    void InteractiveCli::run() {
      std::cout << "Welcome to Iroha-Cli. " << std::endl;
      // Parsing cycle
      while (true) {
        printMenu("Choose what to do:", menu_points_);
        auto line = promptString("> ");
        if (not line) {
          // Line contains terminating symbol
          break;
        }
        parseMain(line.value());
      }
    }

  }  // namespace interactive
}  // namespace iroha_cli
