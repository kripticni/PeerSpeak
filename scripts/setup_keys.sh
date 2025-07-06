#!/usr/bin/env bash 
#!/usr/bin/env bash

# Generate CA
openssl genpkey -algorithm ED25519 -out ca.key
openssl req -x509 -new -nodes -key ca.key -days 3650 -out ca.crt \
  -subj "/" -addext "basicConstraints=critical,CA:TRUE"

# Generate Server Certificate
openssl genpkey -algorithm ED25519 -out server.key
openssl req -new -key server.key -out server.csr -subj "/" \
  -addext "basicConstraints=critical,CA:FALSE" \
  -addext "keyUsage=digitalSignature,keyAgreement" \
  -addext "extendedKeyUsage=serverAuth"
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -days 365 -out server.crt \
  -copy_extensions copyall

# Generate Client Certificate
openssl genpkey -algorithm ED25519 -out client.key
openssl req -new -key client.key -out client.csr -subj "/" \
  -addext "basicConstraints=critical,CA:FALSE" \
  -addext "keyUsage=digitalSignature,keyAgreement" \
  -addext "extendedKeyUsage=clientAuth"
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -days 365 -out client.crt \
  -copy_extensions copyall

openssl verify -CAfile ca.crt server.crt
openssl verify -CAfile ca.crt client.crt

chmod 600 ./*.key
chmod 644 ./*.crt
