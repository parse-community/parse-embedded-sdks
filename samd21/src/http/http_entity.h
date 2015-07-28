/**
 * \file
 *
 * \brief HTTP base entity.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 */

/**
 * \addtogroup sam0_httpc_group
 * @{
 */

#ifndef HTTP_ENTITY_H_INCLUDED
#define HTTP_ENTITY_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * \brief A structure that the implementation of HTTP entity. 
 */
struct http_entity {
	/** A flag for the using the chunked encoding transfer or not. */
	uint8_t is_chunked;
	/** 
	 * \brief Get content mime type. 
	 *
	 * \param[in]  priv_data       Private data of this entity.
	 *
	 * \return     Content type of entity.
	 */
	const char* (*get_contents_type)(void *priv_data);
	/** 
	 * \brief Get content length. 
	 * If using the chunked encoding, This function does not needed. 
	 * 
	 * \param[in]  priv_data       Private data of this entity.
	 *
	 * \return     Content length of entity.
	 */
	int (*get_contents_length)(void *priv_data);
	/** 
	 * \brief Read the content.
	 * 
	 * \param[in]  priv_data       Private data of this entity.
	 * \param[in]  buffer          A buffer that stored read data.
	 * \param[in]  size            Maximum size of the buffer.
	 * \param[in]  written         total size of ever read.
	 *
	 * \return     Read size.
	 */
	int (*read)(void *priv_data, char *buffer, uint32_t size, uint32_t written);
	/** 
	 * \brief Close the entity.
	 * Completed to send request. So release the resource.
	 * 
	 * \param[in]  priv_data       Private data of this entity.
	 */
	void (*close)(void *priv_data);
	/** Private data of this entity. Stored various data necessary for the operation of the entity. */
	void *priv_data;

};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HTTP_ENTITY_H_INCLUDED */
