/**
* @file Texture2D.cpp
* @brief Implementación de la creación, enlace y liberación de texturas 2D.
* @note La carga de imágenes, la conversión desde otra textura y la creación de
*       cubemaps permanecen pendientes en las funciones que retornan @c E_NOTIMPL.
*/
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Device.h"
#include "Rendering/DeviceContext.h"

/** @brief Destructor; libera el recurso y la vista de textura. */
Texture2D::~Texture2D() noexcept {
	destroy();
}

/**
 * @brief Interpreta el tipo de archivo y delega la carga de texturas PNG/JPG.
 * @param device Dispositivo de Direct3D 11.
 * @param textureName Nombre o ruta base de la textura, sin extensión añadida por este método.
 * @param extensionType Tipo de archivo indicado por @c ExtensionType.
 * @return Código @c HRESULT de la operación.
 * @note DDS no ejecuta una ruta de carga en esta implementación. La función auxiliar de
 *       carga de imagen devuelve actualmente @c E_NOTIMPL.
 */
HRESULT
Texture2D::init(Device& device,
	const std::string& textureName,
	ExtensionType extensionType) {
	if (!device.getDevice()) {
		ERROR("Texture", "init", "Device is null.");
		return E_POINTER;
	}
	if (textureName.empty()) {
		ERROR("Texture", "init", "Texture name cannot be empty.");
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	switch (extensionType) {
	case DDS:
		break;
	case PNG: {
		m_textureName = textureName + ".png";
		hr = InitTextureFromImage(device, m_textureName, *this);
		break;
	}
	case JPG: {
		m_textureName = textureName + ".jpg";
		hr = InitTextureFromImage(device, m_textureName, *this);
		break;
	}
	default:
		ERROR("Texture", "init", "Unsupported extension type");
		return E_INVALIDARG;
	}

	return hr;
}

/**
 * @brief Crea una textura 2D vacía con las dimensiones y formato solicitados.
 * @param device Dispositivo de Direct3D 11.
 * @param width Ancho en píxeles.
 * @param height Alto en píxeles.
 * @param Format Formato DXGI del recurso.
 * @param BindFlags Banderas de enlace del recurso.
 * @param sampleCount Cantidad de muestras por píxel.
 * @param qualityLevels Nivel de calidad de muestreo.
 * @return @c S_OK si se crea la textura; de lo contrario, código @c HRESULT.
 */
HRESULT
Texture2D::init(Device& device,
	unsigned int width,
	unsigned int height,
	DXGI_FORMAT Format,
	unsigned int BindFlags,
	unsigned int sampleCount,
	unsigned int qualityLevels) {
	if (!device.getDevice()) {
		ERROR("Texture", "init", "Device is null.");
		return E_POINTER;
	}
	if (width == 0 || height == 0) {
		ERROR("Texture", "init", "Width and height must be greater than 0");
		return E_INVALIDARG;
	}

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = Format;
	desc.SampleDesc.Count = sampleCount;
	desc.SampleDesc.Quality = qualityLevels;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = BindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = device.CreateTexture2D(&desc, nullptr, &m_texture);

	if (FAILED(hr)) {
		ERROR("Texture", "init",
			("Failed to create texture with specified params. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

/**
 * @brief Inicializa una textura a partir de otra con un formato distinto.
 * @param device Dispositivo de Direct3D 11.
 * @param textureRef Textura de referencia.
 * @param format Formato DXGI solicitado.
 * @return @c E_NOTIMPL porque la operación aún no está implementada.
 */
HRESULT Texture2D::init(Device& device, Texture2D& textureRef, DXGI_FORMAT format)
{
	return E_NOTIMPL;
}

/**
 * @brief Carga una imagen desde una ruta y la convierte en recurso de textura.
 * @param device Dispositivo de Direct3D 11.
 * @param fullPath Ruta del archivo.
 * @param texture Textura que debería recibir los recursos.
 * @return @c E_NOTIMPL porque la carga no está implementada en este archivo.
 */
HRESULT Texture2D::InitTextureFromImage(Device& device, const std::string& fullPath, Texture2D& texture)
{
	return E_NOTIMPL;
}

/**
 * @brief Enlaza la vista de textura al Pixel Shader.
 * @param deviceContext Contexto inmediato donde se realizará el enlace.
 * @param StartSlot Slot inicial de recursos.
 * @param NumViews Cantidad de vistas que se asignarán.
 */
void
Texture2D::render(DeviceContext& deviceContext,
	unsigned int StartSlot,
	unsigned int NumViews) {
	if (!deviceContext.getDeviceContext()) {
		ERROR("Texture", "render", "Device Context is null.");
		return;
	}

	if (m_textureFromImg) {
		deviceContext.PSSetShaderResources(StartSlot, NumViews, &m_textureFromImg);
	}
}

/**
 * @brief Libera tanto el recurso de textura como su vista SRV.
 * @post Los punteros @c m_texture y @c m_textureFromImg quedan en @c nullptr.
 */
void
Texture2D::destroy() {
	if (m_texture != nullptr) {
		SAFE_RELEASE(m_texture);
	}
	if (m_textureFromImg != nullptr) {
		SAFE_RELEASE(m_textureFromImg);
	}
}

/**
 * @brief Construye un cubemap usando seis archivos de imagen.
 * @param device Dispositivo de Direct3D 11.
 * @param deviceContext Contexto inmediato de Direct3D 11.
 * @param facePaths Rutas de las seis caras.
 * @param generateMips Indica si se deben generar niveles mip.
 * @return @c E_NOTIMPL porque la creación de cubemaps está pendiente.
 */
HRESULT Texture2D::CreateCubemap(Device& device, DeviceContext& deviceContext, const std::array<std::string, 6>& facePaths, bool generateMips)
{
	return E_NOTIMPL;
}