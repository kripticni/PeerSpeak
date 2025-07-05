#include "asio/use_awaitable.hpp"
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <memory>

asio::ssl::context create_ssl_context() {
  asio::ssl::context ctx(asio::ssl::context::tlsv13);

  ctx.set_options(
    asio::ssl::context::no_sslv2 |
    asio::ssl::context::no_sslv3 |
    asio::ssl::context::no_tlsv1 |
    asio::ssl::context::no_tlsv1_1 |
    asio::ssl::context::no_tlsv1_2 |
    SSL_OP_NO_COMPRESSION |
    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
  );

  /*
    ctx.set_verify_callback([](bool preverified, asio::ssl::verify_context&) {
        return preverified;
    });
    */

  ctx.use_certificate_file("server.crt", asio::ssl::context::pem);
  ctx.use_private_key_file("server.key", asio::ssl::context::pem);

  ctx.load_verify_file("ca.crt");
  ctx.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);

  SSL_CTX_set_ciphersuites(ctx.native_handle(), 
                           "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");

  return ctx;
}

asio::awaitable<void> echo_session(asio::ssl::stream<asio::ip::tcp::socket> ssl_socket) {
  try {
    char data[1024];
    while(true) {
      std::size_t n = co_await ssl_socket.async_read_some(asio::buffer(data), asio::use_awaitable);
      co_await asio::async_write(ssl_socket, asio::buffer(data, n), asio::use_awaitable);
    }
  } catch (const std::exception& e) {
    std::cerr << "Session ended: " << e.what() << "\n";
  }
  co_await ssl_socket.async_shutdown(asio::use_awaitable);
}

// 3. Update listener to handle TLS handshakes
asio::awaitable<void> listener(asio::io_context& ctx, asio::ssl::context& ssl_ctx, uint16_t port) {
  asio::ip::tcp::acceptor acceptor(ctx, {asio::ip::tcp::v4(), port});
  while(true) {
    asio::ip::tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
    auto ssl_socket = asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), ssl_ctx);

    try {
      co_await ssl_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
      co_spawn(ctx, echo_session(std::move(ssl_socket)), asio::detached);
    } catch (const std::exception& e) {
      std::cerr << "Handshake failed: " << e.what() << "\n";
    }
  }
}

int main() {
  try {
    asio::io_context ctx(1);

    auto ssl_ctx = create_ssl_context();
    co_spawn(ctx, listener(ctx, ssl_ctx, 5555), asio::detached);

    ctx.run();
  } catch (const std::exception& e) {
    std::cerr << "Server exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
