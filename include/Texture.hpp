#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <iostream>
#include "Color.hpp" 

class Texture {
private:
    int width, height, channels;
    unsigned char* localBuffer; // Ponteiro para os dados do stb_image

public:
    Texture();
    ~Texture(); // Destrutor para limpar a memória

    bool load(const std::string& filename);
    
    // A mágica: recebe UV (0 a 1) e retorna a Cor
    Color getColor(float u, float v) const;
    
};

#endif