/******************************************************************************
 * Name      : S3A7_REANIBEX_100                                              *
 * Copyright : Osatu S. Coop                                                  *
 * Compiler  : MinGW32                                                        *
 * Target    : Reanibex Series                                                *
 ******************************************************************************/

/*!
 * @file        nx_secure.c
 * @brief       NetX secure for check certificates.
 *              To use these functions you must first call nx_init().
 *
 * @version     v1
 * @date        08/10/2021
 * @author      ivicente
 * @warning     author   date    version     .- Edition
 * @bug
 *
 */


/******************************************************************************
 ** Includes
 */
#include "nx_secure_Mock.h"
#include "synergy_gen_Mock/common_data_Mock.h"



/******************************************************************************
 ** Defines
 */

/******************************************************************************
 ** Typedefs
 */

/******************************************************************************
 ** Constants
 */
#define ASCII_TO_DEC(x)                 (uint8_t) ((x >= '0')? (x-'0'):0)
/******************************************************************************
 ** Externs
 */

/******************************************************************************
 ** Globals
 */

/*static FX_FILE  cert_file;
static FX_FILE  ca_cert_file;

static NX_SECURE_X509_CERT certificate;
static NX_SECURE_X509_CERT ca_certificate;

NX_SECURE_X509_CERTIFICATE_STORE *store;

static DEVICE_DATE_t *pDate;
static DEVICE_TIME_t *pTime;

uint8_t fx_res;
bool_t ret_val;*/

/******************************************************************************
 ** Prototypes
 */

/**
 * @brief   This function conpares certificate validation data with UTC-0 data.
 *          If the certificate has expired or is going to expire on the same day, its return an error
 *
 * @param[in] time_asn1 Certificate data in ASN1 format
 * @param[in] length Certificate length
 *
 * @return #CERT_VALID
 * @return #CERT_NOT_VALID
 */
/*static bool_t certificate_time(const UCHAR *time_asn1, uint16_t length)
{
    uint8_t inc_month = 0;
    uint32_t day = 0, month = 0, year = 0;
    //uint32_t sec = 0, min = 0, hour = 0;
    uint32_t    day_1 = 0, day_2 = 0;
    //uint32_t    sec_1, sec_2;
    char_t c_year[2] = {0};
    char_t c_month[2] = {0};
    char_t c_day[2] = {0};

// comment_1
    char_t c_hour[2] = {0};
    char_t c_min[2] = {0};
    char_t c_sec[2] = {0};
// comment_1

    // read the power on date and time ...
    pDate = Get_Device_Date();
    pTime = Get_Device_Time();

    // ASN1 time format: aaMMddHHmmssZ. The last "Z" (capital) sign means invariant zone (UTC or GMT 0) and MUST be placed at the end of the string.
    memcpy(c_year, time_asn1, length);
    year = (uint32_t)(((ASCII_TO_DEC(c_year[0])*10) + ASCII_TO_DEC(c_year[1]))+2000);

    memcpy(c_month, time_asn1+2, length);
    month = (uint32_t)((ASCII_TO_DEC(c_month[0])*10) + ASCII_TO_DEC(c_month[1]));

    memcpy(c_day, time_asn1+4, length);
    day = (uint32_t)((ASCII_TO_DEC(c_day[0])*10) + ASCII_TO_DEC(c_day[1]));

// comment_2
    memcpy(c_hour, time_asn1+6, length);
    hour = (uint32_t)((ASCII_TO_DEC(c_hour[0])*10) + ASCII_TO_DEC(c_hour[1]));

    memcpy(c_min, time_asn1+8, length);
    min = (uint32_t)((ASCII_TO_DEC(c_min[0])*10) + ASCII_TO_DEC(c_min[1]));

    memcpy(c_sec, time_asn1+10, length);
    sec = (uint32_t)((ASCII_TO_DEC(c_sec[0])*10) + ASCII_TO_DEC(c_sec[1]));
// comment_2

    // day_1: certificate time // day_2: UTC-0
    day_1 = (year  * 365) + ((month  & 0xFF) * 30) + (day  & 0xFF);
    day_2 = ((uint32_t)(pDate->year+2000) * 365) + (((uint32_t)pDate->month & 0xFF) * 30) + ((uint32_t)pDate->date  & 0xFF);

    if (day_1 >= day_2)
    {
        inc_month = pDate->month;
        inc_month++;
        if(inc_month == month && ((uint32_t)pDate->year+2000) == year)
        {
            return CERT_EXP_SOON; // The certificate will expire soon
        }
        return CERT_VALID;
    }
// comment_3
    if(day_1 > day_2)
    {
        sec_1 = ((hour >> 16) * 60 * 60) + (((min >> 8) & 0xFF) * 60) + (sec  & 0xFF);
        sec_2 = ((pTime->hour >> 16) * 60 * 60) + (((pTime->hour >> 8) & 0xFF) * 60) + (pTime->hour  & 0xFF);
        return (sec_1 - sec_2 > 0) ? 0 : 1
    }
// comment_3
    return CERT_NOT_VALID;
}*/

/**
 * @brief   This function initialize NetX system
 *
 * @param
 *
 * @return none
 */
void nx_init_mock(void)
{
	printf("nx_init_mock\n");
    // Initialize the NetX system.
   // nx_system_initialize();
}

/**
 * @brief This function read TLS CA certificate from SD and check the validity of the certificate.
 *        The certificate must be in DER format. DER is a binary encoding for X.509 certificate.
 *
 * @param
 *
 * @return #CERT_VALID
 * @return #CERT_NOT_VALID
 */
/*uint16_t check_TLS_Cacert(void)
{
    uint32_t read_nBytes = 0;
    unsigned char *ca_cert_data;
    uint16_t status;

    fx_res = (uint8_t) fx_directory_default_set(&sd_fx_media, FX_NULL); // Reset default directory

    // Hidden file
    fx_res = (uint8_t) fx_file_attributes_set(&sd_fx_media, TLS_CA_CERT_FILENAME, FX_HIDDEN);

    // Load and read certificate file on SD
    fx_res = (uint8_t) fx_file_open(&sd_fx_media, &ca_cert_file, TLS_CA_CERT_FILENAME, FX_OPEN_FOR_READ);
    if (fx_res == 0)
    {
        ca_cert_data = malloc(sizeof(unsigned char) * (size_t)ca_cert_file.fx_file_current_file_size);
        fx_res = (uint8_t) fx_file_read(&ca_cert_file, ca_cert_data, (ULONG)ca_cert_file.fx_file_current_file_size, &read_nBytes);
    }

    // Initialize an X.509 Certificate for NetX Secure TLS
    status = (uint16_t) nx_secure_x509_certificate_initialize(&ca_certificate, ca_cert_data, (USHORT)(ca_cert_file.fx_file_current_file_size), NX_NULL, 0, NULL, 0, (UINT)NX_SECURE_X509_KEY_TYPE_NONE);

    // Flush data to physical media
    fx_res = (uint8_t) fx_media_flush (&sd_fx_media);

    // Close the file
    fx_res = (uint8_t) fx_file_close(&ca_cert_file);

    if(status == 0x00)
    {
        ret_val = certificate_time(ca_certificate.nx_secure_x509_not_after, 2);
        if(ret_val == 0x00)
        {
            return CERT_VALID;
        }
        if(ret_val == 0x01)
        {
            return CERT_EXP_SOON;
        }
        else return CERT_NOT_VALID;
    }
    return status;
}*/

/**
 * @brief This function read WPA-EAP certificate from SD and check the validity of the certificate.
 *        The certificate must be in DER format. DER is a binary encoding for X.509 certificate.
 *
 * @param
 *
 * @return #CERT_VALID
 * @return #CERT_NOT_VALID
 */
/*uint16_t check_WPA_EAP_Cert(void)
{
    uint32_t read_nBytes = 0;
    unsigned char *cert_data;
    uint16_t status;

    fx_res = (uint8_t) fx_directory_default_set(&sd_fx_media, FX_NULL); // Reset default directory

    // Hidden file
    fx_res = (uint8_t) fx_file_attributes_set(&sd_fx_media, WPA_EAP_CERT_FILENAME, FX_HIDDEN);

    // Load and read certificate file on SD
    fx_res = (uint8_t) fx_file_open(&sd_fx_media, &cert_file, WPA_EAP_CERT_FILENAME, FX_OPEN_FOR_READ);
    if (fx_res == 0)
    {
        cert_data = malloc(sizeof(unsigned char) * (size_t)cert_file.fx_file_current_file_size);
        fx_res = (uint8_t) fx_file_read(&cert_file, cert_data, (ULONG)cert_file.fx_file_current_file_size, &read_nBytes);
    }

    // Initialize an X.509 Certificate for NetX Secure TLS
    status = (uint16_t) nx_secure_x509_certificate_initialize(&certificate, cert_data, (USHORT)(cert_file.fx_file_current_file_size), NX_NULL, 0, NULL, 0, (UINT)NX_SECURE_X509_KEY_TYPE_NONE);

    // Flush data to physical media
    fx_res = (uint8_t) fx_media_flush (&sd_fx_media);

    // Close the file
    fx_res = (uint8_t) fx_file_close(&cert_file);

    if(status == 0x00)
    {
        ret_val = certificate_time(certificate.nx_secure_x509_not_after, 2);
        if(ret_val == 0X00)
        {
            return CERT_VALID;
        }
        if(ret_val == 0x01)
        {
            return CERT_EXP_SOON;
        }
        else return CERT_NOT_VALID;
    }
    return status;
}*/
