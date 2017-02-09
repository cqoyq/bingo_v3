/*
 * asiohandler.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_VISITOR_ASIOHANDLER_H_
#define BINGO_RABBITMQ_VISITOR_ASIOHANDLER_H_

#include <deque>
#include <vector>
#include <memory>

#include <amqpcpp.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include "bingo/log/logger.h"
#include "bingo/string.h"
#include "bingo/define.h"

#include "bingo/log/log_level.h"
#include "bingo/log/log_handler.h"
using namespace bingo::log;

class AmqpBuffer;
class AsioHandler: public AMQP::ConnectionHandler
{
private:

    typedef std::deque<std::vector<char>> OutputBuffers;

    virtual void onData(AMQP::Connection *connection, const char *data, size_t size);
    virtual void onConnected(AMQP::Connection *connection);
    virtual void onError(AMQP::Connection *connection, const char *message);
    virtual void onClosed(AMQP::Connection *connection);

    void doConnect(const std::string& host, uint16_t port);
    void doRead();
    void doWrite();

    void parseData();

private:

    boost::asio::io_service& _ioService;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::deadline_timer _timer;

    std::vector<char> _asioInputBuffer;
    std::shared_ptr<AmqpBuffer> _amqpBuffer;
    AMQP::Connection* _connection;
    OutputBuffers _outputBuffer;
    bool _writeInProgress;
    bool _connected;
    bool _quit;

public:

    static constexpr size_t ASIO_INPUT_BUFFER_SIZE = 4*1024; //4kb

    AsioHandler(boost::asio::io_service& ioService,  log_handler*& logger, bool is_heartbeat = false);

    void connect(const std::string& host, uint16_t port);

    virtual ~AsioHandler();

    AsioHandler(const AsioHandler&) = delete;
    AsioHandler& operator=(const AsioHandler&)=delete;

    bool connected()const;

private:
    log_handler* logger_;

    bool is_heartbeat_;			// Whether heartbeat is valid.
    ptime p1_;						// Save heartbeat datetime.
    deadline_timer heartbeat_timer_;

    void start_heartbeat();
    bool check_heartbeat_timeout();
    void send_heartbeat();
    void set_heartbeat_timer();

    void log_out(int level, const char* message);
};

#endif /* BINGO_RABBITMQ_VISITOR_ASIOHANDLER_H_ */
