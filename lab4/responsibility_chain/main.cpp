#include <vector>
#include <print>
#include <cstdint>
#include <string>
#include <string_view>
#include <functional>
#include <optional>

enum class HttpMethod {
	GET, POST, OPTIONS
};

struct HttpRequest {
	HttpMethod method;
	std::string uri;
};

struct HttpResponse {
	uint32_t status;
};

using HandlerFn = std::function<std::optional<HttpResponse>(const HttpRequest&)>;

struct Chain {
	HandlerFn handler;
	Chain* next; // im too lazy to Box<> it

	HttpResponse run(HttpRequest req) {
		Chain* cur = this;
		while (cur != nullptr) {
			auto resp = cur->handler(req);
			if (resp.has_value()) {
				return resp.value();
			}

			cur = cur->next;
		}

		return {};
	}
};

Chain* create_chain(std::initializer_list<HandlerFn> list) {
	Chain* root = nullptr;
	Chain* current = nullptr;

	for (const HandlerFn& handler : list) {
		if (current == nullptr) {
			root = new Chain { handler, nullptr };
			current = root;
			continue;
		}

		current->next = new Chain { handler, nullptr };
		current = current->next;
	}

	return root;
}

int main() {
	HandlerFn handler_a = [] (const HttpRequest& req) -> std::optional<HttpResponse> {
		std::println("running handler_a");

		if (req.method != HttpMethod::OPTIONS) {
			return std::nullopt;
		}

		return HttpResponse { 200 };
	};

	HandlerFn handler_b = [] (const HttpRequest& req) -> std::optional<HttpResponse> {
		std::println("running handler_b");

		if (req.method != HttpMethod::POST) {
			return std::nullopt;
		}

		return HttpResponse { 200 };
	};

	HandlerFn handler_c = [] (const HttpRequest& req) -> std::optional<HttpResponse> {
		std::println("running handler_c");

		if (req.method != HttpMethod::GET) {
			return std::nullopt;
		}

		return HttpResponse { 201 };
	};

	HandlerFn handler_d = [] (const HttpRequest& req) -> std::optional<HttpResponse> {
		std::println("running handler_d");
		return HttpResponse { 500 };
	};

	Chain* chain = create_chain({ handler_a, handler_b, handler_c, handler_d });

	chain->run(HttpRequest { HttpMethod::GET, "/api/v2/all" });
}
