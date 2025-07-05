#include "asio/use_awaitable.hpp"
#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include <iostream>
#include <string>
#include <stdint.h>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using namespace asio::experimental::awaitable_operators;
using namespace std::literals;

asio::awaitable<void> echo_session(tcp::socket socket) {
    try {
        char data[1024];
        while(true) {
            std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
            co_await asio::async_write(socket, asio::buffer(data, n), use_awaitable);
        }
    } catch (std::exception& e) {
        std::cerr << "Session ended: " << e.what() << "\n";
    }
}

asio::awaitable<void> listener(asio::io_context& ctx, uint16_t port) {
    tcp::acceptor acceptor(ctx, {tcp::v4(), port});
    while(true) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(ctx, echo_session(std::move(socket)), detached);
    }
}

int main() {
    try {
        asio::io_context ctx(1);
        co_spawn(ctx, listener(ctx, 5555), detached);
        ctx.run();
    } catch (std::exception& e) {
        std::cerr << "Server exception: " << e.what() << "\n";
    }
}
