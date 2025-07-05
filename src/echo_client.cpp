#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

asio::awaitable<void> echo_client(asio::io_context& ctx) {
  try {
    tcp::resolver resolver(ctx);
    auto endpoints = 
      co_await resolver.async_resolve("127.0.0.1", "5555", use_awaitable);

    tcp::socket socket(ctx);
    co_await asio::async_connect(socket, endpoints, use_awaitable);

    std::string input;
    while (std::getline(std::cin, input)) {
      input += '\n';
      co_await asio::async_write(socket, asio::buffer(input), use_awaitable);

      char reply[1024];
      std::size_t n = co_await socket.async_read_some(asio::buffer(reply), use_awaitable);
      std::cout << "Server: " << std::string(reply, n);
    }

  } catch (std::exception& e) {
    std::cerr << "Client error: " << e.what() << "\n";
  }
}

int main() {
  try {
    asio::io_context ctx;
    co_spawn(ctx, echo_client(ctx), detached);
    ctx.run();
  } catch (std::exception& e) {
    std::cerr << "Fatal client exception: " << e.what() << "\n";
  }
}

