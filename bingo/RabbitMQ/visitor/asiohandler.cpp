#include <iostream>

#include "asiohandler.h"



bool is_heartbeat(char* data, size_t data_size){

	char heart[8] = {0x08, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0xce};

	if(data[0] == 0x08 && memcmp(heart, data, 8) == 0){
		return true;
	}else{
		return false;
	}
}

using boost::asio::ip::tcp;

class AmqpBuffer
{
public:
    AmqpBuffer(size_t size) :
            _data(size, 0),
            _use(0)
    {
    }

    size_t write(const char* data, size_t size)
    {
        if (_use == _data.size())
        {
            return 0;
        }

        const size_t length = (size + _use);
        size_t write = length < _data.size() ? size : _data.size() - _use;
        memcpy(_data.data() + _use, data, write);
        _use += write;
        return write;
    }

    void drain()
    {
        _use = 0;
    }

    size_t available() const
    {
        return _use;
    }

    const char* data() const
    {
        return _data.data();
    }

    void shl(size_t count)
    {
        assert(count < _use);

        const size_t diff = _use - count;
        std::memmove(_data.data(), _data.data() + count, diff);
        _use = _use - count;
    }

private:
    std::vector<char> _data;
    size_t _use;
};

AsioHandler::AsioHandler(boost::asio::io_service& ioService, log_handler*& logger, bool is_heartbeat) :
        _ioService(ioService),
        _socket(ioService),
        _timer(ioService),
        _asioInputBuffer(ASIO_INPUT_BUFFER_SIZE, 0),
        _amqpBuffer(new AmqpBuffer(ASIO_INPUT_BUFFER_SIZE * 2)),
        _connection(nullptr),
        _writeInProgress(false),
        _connected(false),
        _quit(false),
        logger_(logger),
        is_heartbeat_(is_heartbeat),
        heartbeat_timer_(ioService)

{
}

AsioHandler::~AsioHandler()
{
}

void AsioHandler::connect(const std::string& host, uint16_t port)
{
    doConnect(host, port);
}

void AsioHandler::log_out(int level, const char* message){
	if(logger_){
		string info = message;
		logger_->handle(level, RABBITMQ_ASIOHANDLERLOG_TAG, info);
	}
}

void AsioHandler::start_heartbeat(){

	// Check heartbeat is valid, if value is valid, start to heartbeat-timer.
	if(!is_heartbeat_)
		return;

	boost::posix_time::seconds s(RABBITMQ_MAX_WAIT_FOR_HEARTBEAT_SECONDS);
	boost::posix_time::time_duration td = s;
	heartbeat_timer_.expires_from_now(td);
	heartbeat_timer_.async_wait([this](const boost::system::error_code& ec){
		if(!ec){

#ifdef BINGO_MQ_DEBUG
			// Log output.
			log_out(bingo::log::LOG_LEVEL_DEBUG, "start to check heartbeat!");
#endif

			if(check_heartbeat_timeout()){
				send_heartbeat();
				set_heartbeat_timer();
			}

			start_heartbeat();
		}
	});
}

bool AsioHandler::check_heartbeat_timeout(){
	ptime now = boost::posix_time::microsec_clock::local_time();
	ptime p1 = now - seconds(RABBITMQ_MAX_WAIT_FOR_HEARTBEAT_SECONDS);
	if(p1_ < p1){
		return true;
	}else{
		return false;
	}
}

void AsioHandler::set_heartbeat_timer(){
	p1_ = boost::posix_time::microsec_clock::local_time();
#ifdef BINGO_MQ_DEBUG
	// Log output.
	log_out(bingo::log::LOG_LEVEL_DEBUG, "call set_heartbeat_timer()");
#endif
}

void AsioHandler::send_heartbeat(){
	char heart[8] = {0x08, 0x00, 0x00, 0x00,
							   0x00, 0x00, 0x00, 0xce};
	 boost::asio::async_write(_socket,
				boost::asio::buffer(&heart[0], 8),
				[this](boost::system::error_code ec, std::size_t length )
				{
					 if(!ec)
					{
#ifdef BINGO_MQ_DEBUG
						 // Log output.
						 log_out(bingo::log::LOG_LEVEL_DEBUG, "send heartbeat to server!");
#endif
					}
				}
	 );
}

void AsioHandler::doConnect(const std::string& host, uint16_t port)
{
    tcp::resolver::query query(host, std::to_string(port));
    tcp::resolver::iterator iter = tcp::resolver(_ioService).resolve(query);
    _timer.expires_from_now(boost::posix_time::seconds(15));
    _timer.async_wait([this](const boost::system::error_code& ec){
        if(!ec && !_connected)
        {
        	 // Log output.
        	log_out(bingo::log::LOG_LEVEL_ERROR, "connection timed out!");

            _socket.cancel();
            _ioService.stop();
        }
    });

    boost::asio::async_connect(_socket, iter,
            [this](boost::system::error_code ec, tcp::resolver::iterator)
            {
                _connected = true;
                if (!ec)
                {
                	// Connect success to check heart-jump.
					{
						set_heartbeat_timer();
						start_heartbeat();
					}

                    doRead();

                    if(!_outputBuffer.empty())
                    {
                        doWrite();
                    }
                }
                else
                {
//                    std::cerr<<"Connection error:"<<ec<<std::endl;
                	 // Log output.
                	string err = "connection error, err_no:";
                	err.append(lexical_cast<string>(ec.value()));
                	 log_out(bingo::log::LOG_LEVEL_ERROR, err.c_str());

                	 _ioService.stop();
                }
            });

}

void AsioHandler::onData(
        AMQP::Connection *connection, const char *data, size_t size)
{
    _connection = connection;

    _outputBuffer.push_back(std::vector<char>(data, data + size));
    if (!_writeInProgress && _connected)
    {
        doWrite();
    }
}

void AsioHandler::doRead()
{
    _socket.async_receive(boost::asio::buffer(_asioInputBuffer),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {

                    _amqpBuffer->write(_asioInputBuffer.data(), length);

                    // If package is heartbeat, then update heartbeat time.
                    if(is_heartbeat(_asioInputBuffer.data(), length)){
                    	set_heartbeat_timer();
					}

                    parseData();
                    doRead();
                }
                else
                {
//                    std::cerr<<"Error reading:"<<ec<<std::endl;
//                    exit(1);
                	 // Log output.
					string err = "error reading, err_no:";
					err.append(lexical_cast<string>(ec.value()));
					 log_out(bingo::log::LOG_LEVEL_ERROR, err.c_str());

					 _ioService.stop();
                }
            });
}

void AsioHandler::doWrite()
{
    _writeInProgress = true;
    boost::asio::async_write(_socket,
            boost::asio::buffer(_outputBuffer.front()),
            [this](boost::system::error_code ec, std::size_t length )
            {
                if(!ec)
                {
#ifdef BINGO_MQ_DEBUG
                	 // Log output.
					string info = "send data to server!data_size:";
					info.append(lexical_cast<string>(length));
					 log_out(bingo::log::LOG_LEVEL_DEBUG, info.c_str());
#endif

                    _outputBuffer.pop_front();
                    if(!_outputBuffer.empty())
                    {
                        doWrite();
                    }
                    else
                    {
                        _writeInProgress = false;
                    }

                    if(_quit)
                    {
                        _socket.close();
                    }
                }
                else
                {
//                    std::cerr<<"Error writing:"<<ec<<std::endl;
                    _socket.close();
//                    exit(1);

                    // Log output.
					string err = "error writing, err_no:";
					err.append(lexical_cast<string>(ec.value()));
					 log_out(bingo::log::LOG_LEVEL_ERROR, err.c_str());

					 _ioService.stop();
                }
            });
}

void AsioHandler::parseData()
{
    if (_connection == nullptr)
    {
        return;
    }

    const size_t count = _connection->parse(_amqpBuffer->data(),
            _amqpBuffer->available());

    if (count == _amqpBuffer->available())
    {
        _amqpBuffer->drain();
    }
    else if (count > 0)
    {
        _amqpBuffer->shl(count);
    }
}

void AsioHandler::onConnected(AMQP::Connection *connection)
{
}
bool AsioHandler::connected() const
{
    return _connected;
}

void AsioHandler::onError(AMQP::Connection *connection, const char *message)
{
//    std::cerr << "AMQP error " << message << std::endl;
	 // Log output.
	string err = "AMQP error, err_msg:";
	err.append(message);
	 log_out(bingo::log::LOG_LEVEL_ERROR, err.c_str());
}

void AsioHandler::onClosed(AMQP::Connection *connection)
{
//    std::cout << "AMQP closed connection" << std::endl;
	 // Log output.
	 log_out(bingo::log::LOG_LEVEL_ERROR, "AMQP closed connection");

    _quit = true;
    if (!_writeInProgress)
    {
        _socket.close();
    }
}
