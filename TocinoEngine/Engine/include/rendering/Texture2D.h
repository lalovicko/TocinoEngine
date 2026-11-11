/**
 * @file Texture2D.h
 * @brief Declara la clase Texture2D del subsistema de renderizado.
 * @ingroup core
 */
#pragma once
#include "Engine/Prerequisites.h"

class Device;
class DeviceContext;

/**
 * @class Texture2D
 * @brief Encapsula una textura 2D en Direct3D 11, incluyendo su recurso y vista como Shader Resource.
 *
 * La clase mantiene una referencia al recurso @c ID3D11Texture2D y, cuando existe,
 * una vista @c ID3D11ShaderResourceView para enlazarlo a los shaders.
 *
 * Puede crear texturas vacías en memoria y define una API para carga desde imagen y
 * cubemaps; algunas de esas operaciones continúan sin implementación en este código.
 */
class
    Texture2D {
public:
    /** @brief Construye una textura vacía, sin recursos de GPU. */
    Texture2D() noexcept = default;

    /** @brief Libera el recurso y la vista asociados a la textura. */
    ~Texture2D() noexcept;

    /** @brief Deshabilita la copia para evitar duplicar la propiedad de referencias COM. */
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    /** @brief Deshabilita el movimiento de la instancia. */
    Texture2D(Texture2D&&) = delete;
    Texture2D& operator=(Texture2D&&) = delete;

    /**
     * @brief Inicializa una textura cargada desde archivo.
     *
     * Crea un recurso de textura a partir de un archivo de imagen y genera su
     * @c ShaderResourceView correspondiente para ser usado en shaders.
     *
     * @param device        Dispositivo con el que se creará la textura.
     * @param textureName   Nombre o ruta del archivo de textura.
     * @param extensionType Tipo de extensión de archivo (ej. PNG, JPG, DDS).
     * @return Código @c HRESULT de la operación. Actualmente las rutas PNG y JPG
     *         devuelven @c E_NOTIMPL por la función auxiliar pendiente.
     *
     * @note Los casos PNG y JPG llaman a @c InitTextureFromImage. El caso DDS no realiza
     *       una carga en la implementación actual; además, @c InitTextureFromImage retorna
     *       @c E_NOTIMPL en el código proporcionado, por lo que la carga de imágenes está pendiente.
     */
    HRESULT
        init(Device& device,
            const std::string& textureName,
            ExtensionType extensionType);

    /**
     * @brief Inicializa una textura creada desde memoria.
     *
     * Crea un recurso de textura 2D vacío con un tamaño y formato especificados.
     * Útil para render targets, depth buffers o texturas dinámicas.
     *
     * @param device        Dispositivo con el que se creará la textura.
     * @param width         Ancho de la textura en píxeles.
     * @param height        Alto de la textura en píxeles.
     * @param Format        Formato DXGI de la textura (ej. DXGI_FORMAT_R8G8B8A8_UNORM).
     * @param BindFlags     Banderas de enlace (ej. @c D3D11_BIND_SHADER_RESOURCE, @c D3D11_BIND_RENDER_TARGET).
     * @param sampleCount   Número de muestras para MSAA (por defecto 1 = sin MSAA).
     * @param qualityLevels Niveles de calidad soportados para MSAA.
     * @return @c S_OK si fue exitoso; código @c HRESULT en caso contrario.
     */
    HRESULT
        init(Device& device,
            unsigned int width,
            unsigned int height,
            DXGI_FORMAT Format,
            unsigned int BindFlags,
            unsigned int sampleCount = 1,
            unsigned int qualityLevels = 0);

    /**
     * @brief Inicializa una textura a partir de otra existente.
     *
     * Crea una nueva textura basada en la descripción de @p textureRef,
     * con un formato diferente.
     *
     * @param device     Dispositivo con el que se creará la textura.
     * @param textureRef Referencia a otra textura existente.
     * @param format     Nuevo formato DXGI de la textura.
     * @return @c E_NOTIMPL en la implementación actual.
     * @note La copia/conversión de una textura existente aún no está implementada.
     */
    HRESULT
        init(Device& device, Texture2D& textureRef, DXGI_FORMAT format);

    /**
     * @brief Carga una imagen desde disco y prepara el recurso de textura correspondiente.
     * @param device Dispositivo de Direct3D 11.
     * @param fullPath Ruta completa del archivo de imagen.
     * @param texture Objeto que recibirá el recurso cargado.
     * @return @c E_NOTIMPL en la implementación proporcionada.
     * @note La función está declarada, pero su cuerpo actual no realiza la carga.
     */
    HRESULT
        InitTextureFromImage(Device& device,
            const std::string& fullPath,
            Texture2D& texture);

    /**
     * @brief Asigna la textura al pipeline de render.
     *
     * Llama a @c PSSetShaderResources para establecer la textura como
     * recurso de shader en la etapa de Pixel Shader.
     *
     * @param deviceContext Contexto donde se aplicará la textura.
     * @param StartSlot     Slot inicial en el que se vinculará la textura.
     * @param NumViews      Número de vistas de recurso a asignar (normalmente 1).
     *
     * @pre @c m_textureFromImg debe haberse creado con init().
     */
    void
        render(DeviceContext& deviceContext, unsigned int StartSlot, unsigned int NumViews);

    /**
     * @brief Libera los recursos de la textura.
     *
     * Libera tanto el recurso de textura (@c ID3D11Texture2D) como su
     * @c ShaderResourceView asociado.
     *
     * @post @c m_texture == nullptr y @c m_textureFromImg == nullptr.
     */
    void
        destroy();

    /**
     * @brief Crea una textura cubemap a partir de seis imágenes, una por cara.
     * @param device Dispositivo que creará los recursos.
     * @param deviceContext Contexto utilizado para operaciones de copia o generación de mipmaps.
     * @param facePaths Rutas de las seis caras del cubemap.
     * @param generateMips Indica si se solicita generar niveles mip.
     * @return @c E_NOTIMPL en la implementación proporcionada.
     * @note La creación de cubemaps todavía no está implementada.
     */
    HRESULT
        CreateCubemap(Device& device,
            DeviceContext& deviceContext,
            const std::array<std::string, 6>& facePaths,
            bool generateMips /*= false*/);

    /**
     * @brief Crea una vista SRV que expone una sola cara de una textura tipo cubemap.
     * @param device Dispositivo de Direct3D 11.
     * @param cubemapTex Recurso de textura que contiene las seis caras.
     * @param format Formato DXGI de la vista.
     * @param faceIndex Índice de la cara dentro del arreglo de slices.
     * @param mipLevels Cantidad de niveles mip incluidos en la vista.
     * @return Puntero a la vista creada o @c nullptr si falla la creación.
     * @pre @p device y @p cubemapTex deben ser punteros válidos.
     */
    ID3D11ShaderResourceView*
        CreateCubemapFaceSRV(ID3D11Device* device,
            ID3D11Texture2D* cubemapTex,
            DXGI_FORMAT format,
            unsigned int faceIndex,
            unsigned int mipLevels = 1)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC d{};
        d.Format = format;
        d.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        d.Texture2DArray.MostDetailedMip = 0;
        d.Texture2DArray.MipLevels = mipLevels;       // usa 1 para vista simple
        d.Texture2DArray.FirstArraySlice = faceIndex; // cara
        d.Texture2DArray.ArraySize = 1;               // solo esa cara

        ID3D11ShaderResourceView* srv = nullptr;
        if (FAILED(device->CreateShaderResourceView(cubemapTex, &d, &srv)))
            return nullptr;

        return srv;
    }


    /** @brief Obtiene el recurso de textura sin transferir su propiedad. */
    ID3D11Texture2D*
        GetTexture() const noexcept {
        return m_texture;
    }

    /** @brief Obtiene la dirección del puntero interno para funciones COM de salida. */
    ID3D11Texture2D**
        GetTextureAddress() noexcept {
        return &m_texture;
    }


public:
    /**
     * @brief Recurso base de textura residente en GPU.
     * @note La clase libera esta referencia en @c destroy().
     */
    ID3D11Texture2D* m_texture = nullptr;

    /**
     * @brief Vista de la textura como recurso de shader.
     *
     * Permite acceder a la textura desde Pixel Shader u otros shaders.
     */
    ID3D11ShaderResourceView* m_textureFromImg = nullptr;

    /**
     * @brief Nombre o ruta de la textura (si proviene de archivo).
     */
    std::string m_textureName;
};

