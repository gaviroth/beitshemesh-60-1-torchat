#pragma once

#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/stdx.hpp>
#include <boost/optional.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;