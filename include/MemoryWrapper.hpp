#pragma once
#include <vhdf.hpp>
#include <DataBlock.hpp>

class MemoryWrapper {
private:
    int disk;

    Schema _schema;
    DataBlock block;

    bool schemaLoaded = false;
    bool blockLoaded = false;
    size_t blockId;

    void serializeBlock( char* data ) const {
        char* pos = data;

        // Serialize vector of written records
        for ( size_t i = 0; i < block.registrosEscritos.size()/8; i++) {
            unsigned char output = 0;
            for ( size_t j = 0; j < 8; j++ ) {
                output = output | ( block.registrosEscritos.at((i*8)+j) << j );
            }
            memcpy( pos, &output, sizeof(unsigned char) );
            pos += sizeof(unsigned char) ;
        }

        memcpy( pos, &(block.overflow), sizeof(size_t) );
        pos += sizeof(size_t);

        // Serialize vector of records
        for ( size_t i = 0; i < _schema.regs_por_bloco; i++ ) {
            if ( block.registrosEscritos.at(i) ) {
                memcpy( pos, block.registros.at(i), _schema.tamanho );
            }
            pos += _schema.tamanho;
        }
    };

    void deserializeBlock( const char* data ) {
        if ( !block.initialized ) block.initialize( _schema.tamanho, _schema.regs_por_bloco );
        const char* pos = data;

        // Deserialize vector of written records
        for ( size_t i = 0; i < block.registrosEscritos.size()/8; i++ ) {
            for ( size_t j = 0; j < 8; j++ ) {
                block.registrosEscritos.at((i*8)+j) = *pos & ( 1 << j );
            }
            pos += sizeof(unsigned char) ;
        }

        memcpy( &(block.overflow), pos, sizeof(size_t) );
        pos += sizeof(size_t);

        // Deserialize vector of records
        for ( size_t i = 0; i < block.registros.size(); i++ ) {
            if ( block.registrosEscritos.at(i) ) {
                memcpy( block.registros.at(i), pos, _schema.tamanho );
            }
            pos += _schema.tamanho;
        }
    }

    void loadSchema( ) {
        char buff[vhdf::BLOCK_SIZE];

        vhdf::readBlock(disk, 0, buff);

        _schema.deserialize(buff);
        schemaLoaded = true;
        block.recordSize = _schema.tamanho;
    }

    void allocateBlocks() {

        block.initialize( _schema.tamanho, _schema.regs_por_bloco );
        char buff[vhdf::BLOCK_SIZE];
        serializeBlock(buff);
        for (size_t i = _schema.primeiro_bloco; i <= _schema.ultimo_bloco; i++) {
            vhdf::writeBlock(disk, i, buff);
        }
    }

public:

    MemoryWrapper( int vhd ) {
        disk = vhd;
    }

    MemoryWrapper( int vhd, const Schema &schema ) {
        disk = vhd;
        initializeWithSchema(schema);
    }

    int blockAccessCount = 0;

    Schema getSchema() {
        if (!schemaLoaded) loadSchema();
        return _schema;
    }

    void updateSchema(const Schema &schema) {

        char buff[vhdf::BLOCK_SIZE];

        schema.serialize(buff);

        vhdf::writeBlock(disk, 0, buff);

        _schema = schema;
        schemaLoaded = true;
    }

    void initializeWithSchema( const Schema &schema ) {
        updateSchema(schema);
        allocateBlocks();
    }

    void loadBlock(size_t index) {
        if (!schemaLoaded) loadSchema();
        char buff[vhdf::BLOCK_SIZE];

        vhdf::readBlock(disk, index, &buff);
        if ( index > _schema.ultimo_bloco) block.initialize(_schema.tamanho, _schema.regs_por_bloco);
        else deserializeBlock(buff);

        blockId = index;
        blockAccessCount++;
        blockLoaded = true;
    };

    void commitBlock() {
        char buff[vhdf::BLOCK_SIZE];
 
        if (blockLoaded) {
            serializeBlock(buff);
            vhdf::writeBlock(disk, blockId, &buff);
        }

        blockAccessCount++;
    }

    int getDiskId() {
        return disk;
    }

    size_t getLoadedBlockId() {
        if (blockLoaded) return blockId;
        else return 0;
    }

    DataBlock* operator->() {
        return &block;
    }
};