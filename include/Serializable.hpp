#pragma once

class Serializable {
public:
    virtual size_t serialize_size() const = 0;
    virtual void serialize( char* data ) const = 0;
    virtual void deserialize( const char* data ) = 0;
};