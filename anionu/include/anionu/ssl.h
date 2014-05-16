#ifndef Anionu_SSL_H
#define Anionu_SSL_H


#include "scy/net/sslcontext.h"
#include "scy/net/sslmanager.h"
#include "scy/crypto/x509certificate.h"


namespace scy { 
namespace anio {
	

// Equifax Secure CA
// =================
static const char EquifaxSecureCA[] = 
	"-----BEGIN CERTIFICATE-----" "\n"
	"MIIDIDCCAomgAwIBAgIENd70zzANBgkqhkiG9w0BAQUFADBOMQswCQYDVQQGEwJVUzEQMA4GA1UE" "\n"
	"ChMHRXF1aWZheDEtMCsGA1UECxMkRXF1aWZheCBTZWN1cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5" "\n"
	"MB4XDTk4MDgyMjE2NDE1MVoXDTE4MDgyMjE2NDE1MVowTjELMAkGA1UEBhMCVVMxEDAOBgNVBAoT" "\n"
	"B0VxdWlmYXgxLTArBgNVBAsTJEVxdWlmYXggU2VjdXJlIENlcnRpZmljYXRlIEF1dGhvcml0eTCB" "\n"
	"nzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwV2xWGcIYu6gmi0fCG2RFGiYCh7+2gRvE4RiIcPR" "\n"
	"fM6fBeC4AfBONOziipUEZKzxa1NfBbPLZ4C/QgKO/t0BCezhABRP/PvwDN1Dulsr4R+AcJkVV5MW" "\n"
	"8Q+XarfCaCMczE1ZMKxRHjuvK9buY0V7xdlfUNLjUA86iOe/FP3gx7kCAwEAAaOCAQkwggEFMHAG" "\n"
	"A1UdHwRpMGcwZaBjoGGkXzBdMQswCQYDVQQGEwJVUzEQMA4GA1UEChMHRXF1aWZheDEtMCsGA1UE" "\n"
	"CxMkRXF1aWZheCBTZWN1cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MQ0wCwYDVQQDEwRDUkwxMBoG" "\n"
	"A1UdEAQTMBGBDzIwMTgwODIyMTY0MTUxWjALBgNVHQ8EBAMCAQYwHwYDVR0jBBgwFoAUSOZo+SvS" "\n"
	"spXXR9gjIBBPM5iQn9QwHQYDVR0OBBYEFEjmaPkr0rKV10fYIyAQTzOYkJ/UMAwGA1UdEwQFMAMB" "\n"
	"Af8wGgYJKoZIhvZ9B0EABA0wCxsFVjMuMGMDAgbAMA0GCSqGSIb3DQEBBQUAA4GBAFjOKer89961" "\n"
	"zgK5F7WF0bnj4JXMJTENAKaSbn+2kmOeUJXRmm/kEd5jhW6Y7qj/WsjTVbJmcVfewCHrPSqnI0kB" "\n"
	"BIZCe/zuf6IWUrVnZ9NA2zsmWLIodz2uFHdh1voqZiegDfqnc1zqcPGUIWVEX/r87yloqaKHee95" "\n"
	"70+sB3c4" "\n"
	"-----END CERTIFICATE-----";


inline void initDefaultSSLClient() 
{
	auto ctx = std::shared_ptr<net::SSLContext>(
			new net::SSLContext(
				net::SSLContext::CLIENT_USE, "", "", "",
				net::SSLContext::VERIFY_RELAXED, 9, false, 
				"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"));
	ctx->addVerificationCertificate(crypto::X509Certificate(
		anio::EquifaxSecureCA, strlen(anio::EquifaxSecureCA)));
	net::SSLManager::instance().initializeClient(ctx);	
}


} } // namespace scy::anio


#endif // Anionu_Cert_H