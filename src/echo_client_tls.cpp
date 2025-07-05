#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <string>

asio::ssl::context create_ssl_context() {
    asio::ssl::context ctx(asio::ssl::context::tlsv13); // TLS 1.3 only

    ctx.set_options(
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::no_sslv3 |
        asio::ssl::context::no_tlsv1 |
        asio::ssl::context::no_tlsv1_1 |
        asio::ssl::context::no_tlsv1_2 |
        SSL_OP_NO_COMPRESSION |
        SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
    );

    ctx.use_certificate_file("client.crt", asio::ssl::context::pem);
    ctx.use_private_key_file("client.key", asio::ssl::context::pem);

    ctx.load_verify_file("ca.crt");
    ctx.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);

    SSL_CTX_set_ciphersuites(ctx.native_handle(),
        "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");

    return ctx;
}

asio::awaitable<void> echo_client(asio::io_context& io, asio::ssl::context& ssl_ctx) {
    asio::ssl::stream<asio::ip::tcp::socket> ssl_socket(io, ssl_ctx);
    try {
        asio::ip::tcp::endpoint endpoint(
            asio::ip::make_address("127.0.0.1"), 5555);
        
        co_await ssl_socket.lowest_layer().async_connect(endpoint, asio::use_awaitable);
        co_await ssl_socket.async_handshake(asio::ssl::stream_base::client, asio::use_awaitable);

        std::string input;
        while (std::getline(std::cin, input)) {
            input += '\n';

            co_await asio::async_write(ssl_socket, asio::buffer(input), asio::use_awaitable);
            char reply[1024];
            std::size_t n = co_await ssl_socket.async_read_some(asio::buffer(reply), asio::use_awaitable);
            std::cout << "Server: " << std::string(reply, n);
        }
    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
    }
    co_await ssl_socket.async_shutdown(asio::use_awaitable);
}

int main() {
    try {
        asio::io_context io;
        asio::ssl::context ssl_ctx = create_ssl_context();
        co_spawn(io, echo_client(io, ssl_ctx), asio::detached);
        io.run();
    } catch (std::exception& e) {
        std::cerr << "Fatal client exception: " << e.what() << "\n";
    }
}
