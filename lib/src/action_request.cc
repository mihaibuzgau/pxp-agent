#include <cthun-agent/action_request.hpp>
#include <cthun-agent/errors.hpp>

#define LEATHERMAN_LOGGING_NAMESPACE "puppetlabs.cthun_agent.action_request"
#include <leatherman/logging/logging.hpp>

#include <cassert>

namespace CthunAgent {

ActionRequest::ActionRequest() {
}

ActionRequest::ActionRequest(RequestType type,
                             const CthunClient::ParsedChunks& parsed_chunks)
        : type_ { type },
          job_id_ {},
          parsed_chunks_ { parsed_chunks } {
    init();
}

ActionRequest::ActionRequest(RequestType type,
                             CthunClient::ParsedChunks&& parsed_chunks)
        : type_ { type },
          job_id_ {},
          parsed_chunks_ { parsed_chunks } {
    init();
}

void ActionRequest::setJobId(const std::string& job_id) {
    assert(type_ == RequestType::NonBlocking);
    job_id_ = job_id;
}

const RequestType& ActionRequest::type() const { return type_; }
const std::string& ActionRequest::id() const { return id_; }
const std::string& ActionRequest::sender() const { return sender_; }
const std::string& ActionRequest::transactionId() const { return transaction_id_; }
const std::string& ActionRequest::module() const { return module_; }
const std::string& ActionRequest::action() const { return action_; }
const std::string& ActionRequest::jobId() const { return job_id_; }

const CthunClient::ParsedChunks& ActionRequest::parsedChunks() const {
    return parsed_chunks_;
}

// Private interface

void ActionRequest::init() {
    id_ = parsed_chunks_.envelope.get<std::string>("id");
    sender_ = parsed_chunks_.envelope.get<std::string>("sender");

    LOG_INFO("Validating %1% request %2% by %3%",
             requestTypeNames[type_], id_, sender_);
    LOG_DEBUG("Request %1%:\n%2%", id_, parsed_chunks_.toString());

    validateFormat();

    transaction_id_ = parsed_chunks_.data.get<std::string>("transaction_id");
    module_ = parsed_chunks_.data.get<std::string>("module");
    action_ = parsed_chunks_.data.get<std::string>("action");
}

void ActionRequest::validateFormat() {
    if (!parsed_chunks_.has_data) {
        throw request_format_error { "no data" };
    }
    if (parsed_chunks_.invalid_data) {
        throw request_format_error { "invalid data" };
    }
    // NOTE(ale): currently, we don't support ContentType::Binary
    if (parsed_chunks_.data_type != CthunClient::ContentType::Json) {
        throw request_format_error { "data is not in JSON format" };
    }
}

}  // namespace CthunAgent
