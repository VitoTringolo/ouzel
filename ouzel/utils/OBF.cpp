// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "OBF.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace obf
    {
        // reading
        static uint32_t readInt8(const std::vector<uint8_t>& buffer, uint32_t offset, uint8_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw std::runtime_error("Not enough data");

            result = *reinterpret_cast<const uint8_t*>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readInt16(const std::vector<uint8_t>& buffer, uint32_t offset, uint16_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw std::runtime_error("Not enough data");

            result = decodeBigEndian<uint16_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readInt32(const std::vector<uint8_t>& buffer, uint32_t offset, uint32_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw std::runtime_error("Not enough data");

            result = decodeBigEndian<uint32_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readInt64(const std::vector<uint8_t>& buffer, uint32_t offset, uint64_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw std::runtime_error("Not enough data");

            result = decodeBigEndian<uint64_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readFloat(const std::vector<uint8_t>& buffer, uint32_t offset, float& result)
        {
            if (buffer.size() - offset < sizeof(float))
                throw std::runtime_error("Not enough data");

            result = *reinterpret_cast<const float*>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readDouble(const std::vector<uint8_t>& buffer, uint32_t offset, double& result)
        {
            if (buffer.size() - offset < sizeof(double))
                throw std::runtime_error("Not enough data");

            result = *reinterpret_cast<const double*>(buffer.data() + offset);

            return sizeof(result);
        }

        static uint32_t readString(const std::vector<uint8_t>& buffer, uint32_t offset, std::string& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint16_t))
                throw std::runtime_error("Not enough data");

            uint16_t length = decodeBigEndian<uint16_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw std::runtime_error("Not enough data");

            result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
            offset += length;

            return offset - originalOffset;
        }

        static uint32_t readLongString(const std::vector<uint8_t>& buffer, uint32_t offset, std::string& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint32_t))
                throw std::runtime_error("Not enough data");

            uint32_t length = decodeBigEndian<uint32_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw std::runtime_error("Not enough data");

            result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
            offset += length;

            return offset - originalOffset;
        }

        static uint32_t readByteArray(const std::vector<uint8_t>& buffer, uint32_t offset, std::vector<uint8_t>& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint32_t))
                throw std::runtime_error("Not enough data");

            uint32_t length = decodeBigEndian<uint32_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw std::runtime_error("Not enough data");

            result.assign(reinterpret_cast<const uint8_t*>(buffer.data() + offset),
                          reinterpret_cast<const uint8_t*>(buffer.data() + offset) + length);
            offset += length;

            return offset - originalOffset;
        }

        static uint32_t readObject(const std::vector<uint8_t>& buffer, uint32_t offset, std::map<uint32_t, Value>& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint32_t))
                throw std::runtime_error("Not enough data");

            uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (uint32_t i = 0; i < count; ++i)
            {
                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t key = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(key);

                Value node;

                uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result[static_cast<uint32_t>(key)] = node;
            }

            return offset - originalOffset;
        }

        static uint32_t readArray(const std::vector<uint8_t>& buffer, uint32_t offset, std::vector<Value>& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint32_t))
                throw std::runtime_error("Not enough data");

            uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (uint32_t i = 0; i < count; ++i)
            {
                Value node;
                uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result.push_back(node);
            }

            return offset - originalOffset;
        }

        static uint32_t readDictionary(const std::vector<uint8_t>& buffer, uint32_t offset, std::map<std::string, Value>& result)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(uint32_t))
                throw std::runtime_error("Not enough data");

            uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (uint32_t i = 0; i < count; ++i)
            {
                if (buffer.size() - offset < sizeof(uint16_t))
                    throw std::runtime_error("Not enough data");

                uint16_t length = decodeBigEndian<uint16_t>(buffer.data() + offset);

                offset += sizeof(length);

                if (buffer.size() - offset < length)
                    throw std::runtime_error("Not enough data");

                std::string key(reinterpret_cast<const char*>(buffer.data() + offset), length);
                offset += length;

                Value node;

                uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result[key] = node;
            }

            return offset - originalOffset;
        }

        // writing
        static uint32_t writeInt8(std::vector<uint8_t>& buffer, uint8_t value)
        {
            buffer.push_back(value);

            return sizeof(value);
        }

        static uint32_t writeInt16(std::vector<uint8_t>& buffer, uint16_t value)
        {
            uint8_t data[sizeof(value)];

            encodeBigEndian<uint16_t>(data, value);

            buffer.insert(buffer.end(), std::begin(data), std::end(data));

            return sizeof(value);
        }

        static uint32_t writeInt32(std::vector<uint8_t>& buffer, uint32_t value)
        {
            uint8_t data[sizeof(value)];

            encodeBigEndian<uint32_t>(data, value);

            buffer.insert(buffer.end(), std::begin(data), std::end(data));

            return sizeof(value);
        }

        static uint32_t writeInt64(std::vector<uint8_t>& buffer, uint64_t value)
        {
            uint8_t data[sizeof(value)];

            encodeBigEndian<uint64_t>(data, value);

            buffer.insert(buffer.end(), std::begin(data), std::end(data));

            return sizeof(value);
        }

        static uint32_t writeFloat(std::vector<uint8_t>& buffer, float value)
        {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t*>(&value),
                          reinterpret_cast<const uint8_t*>(&value) + sizeof(value));

            return sizeof(float);
        }

        static uint32_t writeDouble(std::vector<uint8_t>& buffer, double value)
        {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t*>(&value),
                          reinterpret_cast<const uint8_t*>(&value) + sizeof(value));

            return sizeof(double);
        }

        static uint32_t writeString(std::vector<uint8_t>& buffer, const std::string& value)
        {
            uint8_t lengthData[sizeof(uint16_t)];

            encodeBigEndian<uint16_t>(lengthData, static_cast<uint16_t>(value.length()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t*>(value.data()),
                          reinterpret_cast<const uint8_t*>(value.data()) + value.length());
            size += static_cast<uint32_t>(value.length());

            return size;
        }

        static uint32_t writeLongString(std::vector<uint8_t>& buffer, const std::string& value)
        {
            uint8_t lengthData[sizeof(uint32_t)];

            encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.length()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t*>(value.data()),
                          reinterpret_cast<const uint8_t*>(value.data()) + value.length());
            size += static_cast<uint32_t>(value.length());

            return size;
        }

        static uint32_t writeByteArray(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& value)
        {
            uint8_t lengthData[sizeof(uint32_t)];

            encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(), value.begin(), value.end());
            size += static_cast<uint32_t>(value.size());

            return size;
        }

        static uint32_t writeObject(std::vector<uint8_t>& buffer, const std::map<uint32_t, Value>& value)
        {
            uint8_t lengthData[sizeof(uint32_t)];

            encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            uint32_t size = sizeof(lengthData);

            for (const auto& i : value)
            {
                uint8_t keyData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(keyData, i.first);

                buffer.insert(buffer.end(), std::begin(keyData), std::end(keyData));

                size += sizeof(keyData);

                size += i.second.encode(buffer);
            }

            return size;
        }

        static uint32_t writeArray(std::vector<uint8_t>& buffer, const std::vector<Value>& value)
        {
            uint8_t lengthData[sizeof(uint32_t)];

            encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            uint32_t size = sizeof(lengthData);

            for (const auto& i : value)
                size += i.encode(buffer);

            return size;
        }

        static uint32_t writeDictionary(std::vector<uint8_t>& buffer, const std::map<std::string, Value>& value)
        {
            uint8_t sizeData[sizeof(uint32_t)];

            encodeBigEndian<uint32_t>(sizeData, static_cast<uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(sizeData), std::end(sizeData));

            uint32_t size = sizeof(sizeData);

            for (const auto& i : value)
            {
                uint8_t lengthData[sizeof(uint16_t)];

                encodeBigEndian<uint16_t>(lengthData, static_cast<uint16_t>(i.first.length()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                size += sizeof(lengthData);

                buffer.insert(buffer.end(),
                              reinterpret_cast<const uint8_t*>(i.first.data()),
                              reinterpret_cast<const uint8_t*>(i.first.data()) + i.first.length());
                size += static_cast<uint32_t>(i.first.length());

                size += i.second.encode(buffer);
            }

            return size;
        }

        uint32_t Value::decode(const std::vector<uint8_t>& buffer, uint32_t offset)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < 1)
                throw std::runtime_error("Not enough data");

            Marker marker = *reinterpret_cast<const Marker*>(buffer.data() + offset);
            offset += 1;

            uint32_t ret = 0;

            switch (marker)
            {
                case Marker::NONE:
                {
                    type = Type::NONE;
                    break;
                }
                case Marker::INT8:
                {
                    type = Type::INT;

                    uint8_t int8Value;
                    ret = readInt8(buffer, offset, int8Value);

                    intValue = int8Value;
                    break;
                }
                case Marker::INT16:
                {
                    type = Type::INT;

                    uint16_t int16Value;
                    ret = readInt16(buffer, offset, int16Value);

                    intValue = int16Value;
                    break;
                }
                case Marker::INT32:
                {
                    type = Type::INT;

                    uint32_t int32Value;
                    ret = readInt32(buffer, offset, int32Value);

                    intValue = int32Value;
                    break;
                }
                case Marker::INT64:
                {
                    type = Type::INT;

                    ret = readInt64(buffer, offset, intValue);
                    break;
                }
                case Marker::FLOAT:
                {
                    type = Type::FLOAT;

                    float floatValue;
                    ret = readFloat(buffer, offset, floatValue);

                    doubleValue = floatValue;
                    break;
                }
                case Marker::DOUBLE:
                {
                    type = Type::DOUBLE;

                    ret = readDouble(buffer, offset, doubleValue);
                    break;
                }
                case Marker::STRING:
                {
                    type = Type::STRING;

                    ret = readString(buffer, offset, stringValue);
                    break;
                }
                case Marker::LONG_STRING:
                {
                    type = Type::STRING;

                    ret = readLongString(buffer, offset, stringValue);
                    break;
                }
                case Marker::BYTE_ARRAY:
                {
                    type = Type::BYTE_ARRAY;

                    ret = readByteArray(buffer, offset, byteArrayValue);
                    break;
                }
                case Marker::OBJECT:
                {
                    type = Type::OBJECT;

                    ret = readObject(buffer, offset, objectValue);
                    break;
                }
                case Marker::ARRAY:
                {
                    type = Type::ARRAY;

                    ret = readArray(buffer, offset, arrayValue);
                    break;
                }
                case Marker::DICTIONARY:
                {
                    type = Type::DICTIONARY;

                    ret = readDictionary(buffer, offset, dictionaryValue);
                    break;
                }
                default:
                    throw std::runtime_error("Unsupported marker");
            }

            offset += ret;

            return offset - originalOffset;
        }

        uint32_t Value::encode(std::vector<uint8_t>& buffer) const
        {
            uint32_t size = 0;

            uint32_t ret = 0;

            switch (type)
            {
                case Type::NONE:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::NONE));
                    size += 1;
                    break;
                }
                case Type::INT:
                {
                    if (intValue > std::numeric_limits<uint32_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::INT64));
                        size += 1;
                        ret = writeInt64(buffer, intValue);
                    }
                    else if (intValue > std::numeric_limits<uint16_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::INT32));
                        size += 1;
                        ret = writeInt32(buffer, static_cast<uint32_t>(intValue));
                    }
                    else if (intValue > std::numeric_limits<uint8_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::INT16));
                        size += 1;
                        ret = writeInt16(buffer, static_cast<uint16_t>(intValue));
                    }
                    else
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::INT8));
                        size += 1;
                        ret = writeInt8(buffer, static_cast<uint8_t>(intValue));
                    }
                    break;
                }
                case Type::FLOAT:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::FLOAT));
                    size += 1;
                    ret = writeFloat(buffer, static_cast<float>(doubleValue));
                    break;
                }
                case Type::DOUBLE:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::DOUBLE));
                    size += 1;
                    ret = writeDouble(buffer, doubleValue);
                    break;
                }
                case Type::STRING:
                {
                    if (stringValue.length() > std::numeric_limits<uint16_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::LONG_STRING));
                        size += 1;
                        ret = writeLongString(buffer, stringValue);
                    }
                    else
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::STRING));
                        size += 1;
                        ret = writeString(buffer, stringValue);
                    }
                    break;
                }
                case Type::BYTE_ARRAY:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::BYTE_ARRAY));
                    size += 1;
                    ret = writeByteArray(buffer, byteArrayValue);
                    break;
                }
                case Type::OBJECT:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::OBJECT));
                    size += 1;
                    ret = writeObject(buffer, objectValue);
                    break;
                }
                case Type::ARRAY:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::ARRAY));
                    size += 1;
                    ret = writeArray(buffer, arrayValue);
                    break;
                }
                case Type::DICTIONARY:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::DICTIONARY));
                    size += 1;
                    ret = writeDictionary(buffer, dictionaryValue);
                    break;
                }
                default:
                    throw std::runtime_error("Unsupported type");
            }

            size += ret;

            return size;
        }
    } // namespace obf
} // namespace ouzel
