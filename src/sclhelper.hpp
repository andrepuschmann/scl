/*__________________________________
 |       _         _         _      |
 |     _( )__    _( )__    _( )__   |
 |   _|     _| _|     _| _|     _|  |
 |  (_   S (_ (_   C (_ (_   L (_   |
 |    |_( )__|  |_( )__|  |_( )__|  |
 |                                  |
 | Signaling and Communication Link |
 |__________________________________|

 SCL Helper Functions
 The purpose is to abstract creation of zmq-compliant messages out
 of strings or even complete protocol buffer objects.
 The constructor takes an existing zmq socket as argument which is
 used for sending or receiving.

 Copyright (C) 2010 Tobias Simon, Andre Puschmann, Mikhail Tarasov,
                    Jin Yang, Ilmenau University of Technology

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */


#ifndef __SCLHELPER_HPP__
#define __SCLHELPER_HPP__


#include <google/protobuf/message.h>
#include "socketmap.hpp"

using namespace std;

class sclGate
{
public:
    //! constructor
    sclGate(SocketMap *map, const string gate)
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        if(map == NULL)
        {
            throw SocketMapException("The SocketMap was not initialized");
        }
        d_zmqSocket = (*map)[string(gate)];
        if (d_zmqSocket == NULL)
        {
            throw SocketMapException("Gate " + gate + " not found in system config");
        }
    };
    //! destructor
    ~sclGate() { /* Todo */ };

    //!  Convert string to 0MQ string and send to socket
    bool sendString(const std::string &string)
    {
        zmq::message_t message(string.size());
        memcpy(message.data(), string.data(), string.size());

        bool rc = d_zmqSocket->send(message);
        return (rc);
    };


    //!  Convert protobuf object to 0MQ string and send to socket
    bool sendProto(const ::google::protobuf::Message &protobuf)
    {
        std::string buffer;
        if (!protobuf.SerializeToString(&buffer)) {
            throw SocketMapException("Failed to serialize a protobuf message");
        }

        zmq::message_t message(buffer.size());
        memcpy(message.data(), buffer.data(), buffer.size());

        bool rc = d_zmqSocket->send(message);
        return (rc);
    };


    /**
     * Receive 0MQ message and convert into protobuf object
     * @return 0 if message was received, -1 otherwise
     */
    int recvProto(::google::protobuf::Message &protobuf, bool blocking = true)
    {
        std::string buffer;


        this->recvString(buffer, blocking);
        if (buffer.size() == 0)
            return -1;

        if (!protobuf.ParseFromString(buffer)) {
            throw SocketMapException("Failed to parse protobuf message");
        }
        return 0;
    };


    //! Receive 0MQ message and convert into std::string
    int recvString(std::string& result, bool blocking = true)
    {
        if (d_zmqSocket != NULL) {
            zmq::message_t message;
            int flags = (blocking == false) ? ZMQ_NOBLOCK : 0;
            try {
                if (d_zmqSocket->recv(&message, flags) == true) {
                    // msg received correctly
                    if (message.size() > 0) {
                        std::string str(static_cast<char*>(message.data()), message.size());
                        result = str;
                        return 0;
                    }
                }
            } catch (const zmq::error_t& e) {
                string error("Failed to receive zeromq message: ");
                error.append(e.what());
                std::cout << error << std::endl;
            }
        }
        return -1;
    };

private:
    // members for ZMQ message handling
    zmq::socket_t *d_zmqSocket;
};


/**
 * The GateFactory class provides a user-friendly way to create gate
 * objects. Only one factory object exists in one address space (singleton).
 * The class however may create one or more SocketMap objects according to
 * the number of SCL components living in one process.
 */
class GateFactory
{
    private:
        static GateFactory* instance;
        GateFactory() {}
        GateFactory(const GateFactory&) {}
        ~GateFactory() {
            // delete elements in component and gate map
            for (std::map<std::string, sclGate*>::iterator it = gateMap.begin(); it != gateMap.end(); ++it) {
                delete it->second;
            }
            for (std::map<std::string, SocketMap*>::iterator it = componentMap.begin(); it != componentMap.end(); ++it) {
                delete it->second;
            }
        }
        std::map<std::string, SocketMap*> componentMap;
        std::map<std::string, sclGate*> gateMap;
    public:
        static GateFactory& getInstance()
        {
            static GateFactory _instance;
            return _instance;
            /*
            if (!instance)
                instance = new GateFactory();
            return *instance;*/
        };
        static void destroy() {
            if (instance)
                delete instance;
            instance = 0;
        };

        /**
         * create gate to send the data through it by providing the name of socket and the name of gate
         * @param gate the name of the gate
         * @param socket the name of the socket
         */
        sclGate *createGate(std::string componentName, std::string gateName) {
            SocketMap *map;
            // try to find existing map for component
            if (componentMap.find(componentName) == componentMap.end()) {
                // component not found, create new socketmap and add it to componentmap
                cout << "Map not found for component " << componentName << endl;
                map = new SocketMap(componentName);
                componentMap[componentName] = map;
            } else {
                // return reference to existing SocketMap
                cout << "Map found for component " << componentName << endl;
                map = componentMap[componentName];
            }
            // try to find existing Gate
            if (gateMap.find(gateName) == gateMap.end()) {
                // gate not found, create new gate and add it to the gatemap
                //cout << "gate " << gateName << " not found, create new" << endl;
                sclGate *gate = new sclGate(map, gateName);
                gateMap[gateName] = gate;
                return gate;
            } else {
                // return pointer to existing gate
                //cout << "gate " << gateName << " found, return pointer" << endl;
                return gateMap[gateName];
            }
        }
};


#endif /* __SCLHELPER_HPP__ */

