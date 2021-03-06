# Solace OpenMAMA "Hello World"

* [Installation on Linux](#installation-on-Linux)

---

## Installation on Linux

Download pre-packaged OpenMAMA from the [OpenMAMA GitHub releases page](https://github.com/OpenMAMA/OpenMAMA/releases):

```
$ wget https://github.com/OpenMAMA/OpenMAMA/releases/download/OpenMAMA-2.4.1-release/openmama-2.4.1-1.el7.centos.x86_64.rpm
```

Install it with `rpm`:

```
$ sudo rpm --install openmama-2.4.1-1.el7.centos.x86_64.rpm
```

Files will be installed into `/opt/openmama` directory:
```
$ ls /opt/openmama
bin config COPYING data examples includes lib
```

Configuring of the installation means editing and referring the `mama.properties` configuration file at `/opt/openamama/config`.

If you would try to run any of the files in the OpenMAMA `bin` directory, you will get error `“cannot open shared object file”`. This is because all OpenMAMA shared libraries are located in `/opt/openmama/lib` and need to be referred to explicitly.

```
$ export LD_LIBRARY_PATH=/opt/openmama/lib
$ /opt/openmama/bin/mamasubscriberc –help
This sample application demonstrates how to subscriber and process
mamaMsg’s from a basic subscription.
```

The most important configuration parameter of OpenMAMA is the middleware bridge.

Bridge is what communicates with a specific messaging broker, and different bridges implementation details are hidden by OpenMAMA interfaces and entities. This is what makes OpenMAMA so powerful: applications are written without being coupled to specific messaging brokers.

Out-of-box `mama.properties` refers to the **Apache Qpid** middleware bridge, and most of the OpenMAMA sample applications by default refer to a different (proprietary) bridge, but we’re going to use the **Solace middleware bridge** that enables communication with a **Solace message router**.

Download the Solace middleware bridge implementation from its location that was given to you by **Solace Customer Support**.

```
$ wget <…location...>/solopenmama_bridge_Linux26-x86_64_opt_7.2.0.165.tar.gz
```

Untar it into a dedicated location and append this location **lib** directory to **LD_LIBRARY_PATH**.

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<…>/solopenmama_bridge/lib
```

The Solace middleware bridge implementation has a dependency on **Solace Messaging API for C**. Download it from location that was given to you by **Solace Customer Support**.

```
$ wget <…location…>/solclient_Linux26-x86_64_opt_7.2.0.489.tar.gz
```

Untar it into a dedicated location and append this location **lib** directory to **LD_LIBRARY_PATH**.

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<…>/solclient/lib
```

Use `ldd` to verify dependences:

```
$ ldd topicPublishOne
    linux-vdso.so.1 =>  (0x00007ffcf53d4000)
    libmama.so => not found
    libc.so.6 => /lib64/libc.so.6 (0x00007f38bc75d000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f38bcb3a000)

$ export LD_LIBRARY_PATH=/opt/openmama/lib
$ ldd topicPublishOne
    linux-vdso.so.1 =>  (0x00007ffe429fd000)
    libmama.so => /opt/openmama/lib/libmama.so (0x00007f9ed356d000)
    libc.so.6 => /lib64/libc.so.6 (0x00007f9ed3191000)
    libm.so.6 => /lib64/libm.so.6 (0x00007f9ed2e8f000)
    librt.so.1 => /lib64/librt.so.1 (0x00007f9ed2c87000)
    libdl.so.2 => /lib64/libdl.so.2 (0x00007f9ed2a82000)
    libpthread.so.0 => /lib64/libpthread.so.0 (0x00007f9ed2866000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f9ed3810000)
```

If you have any issues installing OpenMAMA, ask OpenMAMA developers and users at [Gitter OpenMAMA room](https://gitter.im/OpenMAMA/OpenMAMA).

If you have any issues installing Solace components, check the [Solace community](http://dev.solacesystems.com/community/) for answers to common issues or contact **Solace Customer Support**.
