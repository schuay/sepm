#!/bin/sh
# sdc.sh
#
# Helper script to build a sdc infrastructure

OPENSSL=/usr/bin/openssl
CRTDIR=$HOME/.config/sdc

usage() {
    echo "USAGE:"
    echo
    echo "  $0 user <host> <username>"
    echo "    Generate every everything that's needed for one user at a server."
    echo
    echo "  $0 server <host>"
    echo "    Generate everything that's needed for a server."
    echo
    echo "  $0 genrsa [<username>]"
    echo "    Generate RSA key pair."
    echo
    echo "  $0 genca"
    echo "    Create a CA certificate."
    echo
    echo "  $0 gencert <host>"
    echo "    Create a server certificate."
    echo
    echo "  $0 genconfig <host>"
    echo "    Create a config file for a server."
    exit $1
}

wrapcall() {
    echo -n "$1: "
    $2 > /dev/null 2>&1
    result=$?
    if [ $result ]; then
        echo "DONE"
    else
        echo "FAIL"
        exit 1
    fi
    return $result
}

testca() {
    if [ -f $CRTDIR/ca.crt ] && [ -f $CRTDIR/ca.key ]; then
        return 0
    fi
    return 1
}

testserver() {
    if [ -f $CRTDIR/$1.crt ] && [ -f $CRTDIR/$1.key ] && [ -f $CRTDIR/$1.conf ]; then
        return 0
    fi
    return 1
}

user() {
    if ! testserver $1; then
        create_server $1
    fi
    genrsa "$2@$1"
    clientconfig $1 $2
}

genrsa() {
    PRIVKEY="private.pem"
    PUBKEY="public.pem"
    if [ $1 ]; then
        PRIVKEY="private_$1.pem"
        PUBKEY="public_$1.pem"
    fi
    wrapcall "Generate RSA private key" "$OPENSSL genrsa -out $CRTDIR/$PRIVKEY 4096 -batch"
    wrapcall "Fetch RSA public key" "$OPENSSL rsa -pubout -in $CRTDIR/$PRIVKEY -out $CRTDIR/$PUBKEY"
}

genca() {
    wrapcall "Generate RSA key for CA" "$OPENSSL genrsa -out $CRTDIR/ca.key 4096 -batch"
    wrapcall "Create CA certificate" "$OPENSSL req -new -x509 -days 365 -key $CRTDIR/ca.key -out $CRTDIR/ca.crt -batch -subj /CN=CA/OU=INSO/O=ViennaUT"
}

gencert() {
    if ! testca; then echo "Missing CA"; exit 1; fi
    wrapcall "Generate RSA key for server $1" "$OPENSSL genrsa -out $CRTDIR/$1.key 4096 -batch"
    wrapcall "Create a request for signing" "$OPENSSL req -new -key $CRTDIR/$1.key -out $CRTDIR/$1.csr -batch -subj /CN=$1/OU=INSO/O=ViennaUT"
    wrapcall "Create server certificate" "$OPENSSL x509 -req -days 365 -in $CRTDIR/$1.csr -CA $CRTDIR/ca.crt -CAkey $CRTDIR/ca.key -set_serial 01 -out $CRTDIR/$1.crt"
}

genconfig() {
    echo -n "Generate config for server $1: "

    (
    echo "[server]"
    echo "hostname=$1"
    echo "cafile=$CRTDIR/ca.crt"
    echo "certfile=$CRTDIR/$1.crt"
    echo "keyfile=$CRTDIR/$1.key"

    echo "[db]"
    echo "host=localhost"
    echo "port=5432"
    echo "dbname=$1"
    echo "username=$1"
    echo "password=<password>"
    ) > $CRTDIR/$1.conf

    ln -sf $CRTDIR/$1.conf $CRTDIR/server.conf

    echo "DONE"
}

clientconfig() {
    echo -n "Generate client config: "

    (
    echo "[client]"
    echo "hostname=$1"
    echo "username=$2@$1"
    echo "password=<password>"
    echo "pubkey=$CRTDIR/public_$2@$1.pem"
    echo "privkey=$CRTDIR/private_$2@$1.pem"
    echo "cafile=$CRTDIR/ca.crt"
    ) > $CRTDIR/client.conf

    echo "DONE"
}

create_server() {
    if ! testca; then genca; fi
    gencert $1
    genconfig $1
}

if [ ! -d $CRTDIR ]; then
    mkdir -p $CRTDIR
fi

case $1 in
    genca)
        genca
        ;;
    genrsa)
        user="$2"
        genrsa $user
        ;;
    gencert)
        [ -n "$2" ] || usage 1
        gencert $2
        ;;
    genconfig)
        [ -n $2 ] || usage 1
        host="$2"
        genconfig $host
        ;;
    server)
        ([ -n $2 ] && [ -n $3 ]) || usage 1
        host="$2"
        create_server $host
        ;;
    user)
        ([ -n $2 ] && [ -n $3 ]) || usage 1
        host="$2"
        user="$3"
        user $host $user
        ;;
    *)
        usage 0
        ;;
esac

