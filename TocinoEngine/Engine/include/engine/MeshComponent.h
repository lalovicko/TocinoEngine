/**
 * @file MeshComponent.h
 * @brief Declara el contenedor de datos geométricos de una malla del motor.
 * @ingroup core
 */
#pragma once
#include "Engine/Prerequisites.h"

 /**
  * @class MeshComponent
  * @brief Contenedor de datos geométricos de una malla utilizada por el motor.
  *
  * Un @c MeshComponent agrupa los vértices, índices, nombre y transformación local.
  * No crea recursos de GPU por sí mismo; almacena datos que pueden utilizarse para construir buffers.
  *
  * La malla incluye:
  * - Lista de vértices con la estructura @c Vertex (posición y color RGBA).
  * - Lista de índices que definen las primitivas (triángulos, líneas).
  * - Contadores de vértices e índices.
  */
class
    MeshComponent {
public:
    /**
     * @brief Construye una malla vacía e inicializa sus contadores en cero.
     */
    MeshComponent() : m_numVertex(0), m_numIndex(0) {}

public:
    /**
     * @brief Nombre de la malla.
     */
    std::string m_name;
    /**
     * @brief Matriz de transformación local de la malla, inicializada como identidad.
     */
    DirectX::XMFLOAT4X4 m_localTransform = DirectX::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    /**
     * @brief Lista de vértices de la malla.
     */
    std::vector<Vertex> m_vertex;
    //std::vector<SkyboxVertex> m_skyVertex;

    /**
     * @brief Lista de índices que definen las primitivas de la malla.
     */
    std::vector<unsigned int> m_index;

    /**
     * @brief Contador de vértices de la malla. Debe mantenerse sincronizado con @c m_vertex.
     */
    int m_numVertex;

    /**
     * @brief Contador de índices de la malla. Debe mantenerse sincronizado con @c m_index.
     */
    int m_numIndex;
};

