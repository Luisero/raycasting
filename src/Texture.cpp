#include "../include/Texture.hpp"

// Define necessário para o stb_image funcionar
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <algorithm> // Para std::clamp
#include <cmath>

Texture::Texture() : width(0), height(0), channels(0), localBuffer(nullptr) {}

Texture::~Texture() {
    if (localBuffer) {
        stbi_image_free(localBuffer);
    }
}

bool Texture::load(const std::string& filename) {
    // CORREÇÃO 1: Carrega 4 canais (RGBA) para detectar transparência
    localBuffer = stbi_load(filename.c_str(), &width, &height, &channels, 4);

    if (!localBuffer) {
        std::cerr << "ERRO: Nao foi possivel carregar textura: " << filename << std::endl;
        return false;
    }

    std::cout << "Textura carregada: " << filename << " (" << width << "x" << height << ") Canais: " << channels << "\n";
    return true;
}

Color Texture::getColor(float u, float v) const {
    // 1. Segurança: Se não carregou, retorna Rosa (Erro visível)
    if (!localBuffer) return Color(255, 0, 255);

    // 2. Tiling (Repete a textura se passar de 1.0)
    u = u - std::floor(u);
    v = v - std::floor(v);

    // 3. Mapeia para Pixels
    int x = static_cast<int>(u * (width - 1));
    
    // ATENÇÃO: Se a textura estiver invertida verticalmente, troque a linha comentada:
    // int y = static_cast<int>(v * (height - 1)); 
    int y = static_cast<int>((1.0f - v) * (height - 1));

    // CORREÇÃO 2: Segurança Extrema (Clamping)
    // Garante que x e y nunca saiam da imagem (evita ler lixo de memória)
    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    // 4. Calcula índice (Multiplica por 4 pois agora é RGBA)
    int pixelIndex = (y * width + x) * 4;

    // 5. Pega os valores
    unsigned char r = localBuffer[pixelIndex + 0];
    unsigned char g = localBuffer[pixelIndex + 1];
    unsigned char b = localBuffer[pixelIndex + 2];
    unsigned char a = localBuffer[pixelIndex + 3]; // Alpha

    // CORREÇÃO 3: Teste de Transparência
    // Se o pixel for transparente, retornamos uma cor de debug (ex: Ciano)
    // Se isso aparecer na tela, sabemos que ali é um "buraco" na textura.
    if (a < 10) {
        return Color(0, 255, 255); // Ciano para Debug de Transparência
    }

    return Color(r, g, b);
}