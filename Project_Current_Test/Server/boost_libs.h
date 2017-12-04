#pragma once

// Any boost library that we will use for the server program can be included in this one file

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/find.hpp>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

