# Secure Partition
In order to store the keys in a secure memory partition, TF-M is used, which allows the use of ARM TrustZone. This involves a modification to the flow of calls to the `bls_hsm` functions.

When TF-M is disabled, `bls_hsm_ns` directly calls the functions of `bls_hsm` (which contain all the functionalities used from blst library). When TF-M is enabled, `bls_hsm_ns` calls the `secure_partition_interface` functions, which are responsible for calling the functions running in a secure partition that are in `secure_partition`. It's `secure_partition` which, from the secure world, calls `bls_hsm` functions.

The sequence diagrams below show which keygen calls are made, depending on whether or not TF-M is used.

## Sequence diagram of keygen when TF-M is not enabled
```mermaid
sequenceDiagram
    participant main
    participant bls_hsm_ns
    participant bls_hsm
    
    main->>+bls_hsm_ns: keygen()
    
    bls_hsm_ns->>+bls_hsm: get_keystore()
    bls_hsm-->>-bls_hsm_ns: keystore_size
    
    bls_hsm_ns->>+bls_hsm: secure_keygen()
    bls_hsm-->>-bls_hsm_ns: index
    
    bls_hsm_ns->>+bls_hsm: get_key()
    bls_hsm-->>-bls_hsm_ns: pk
    
    bls_hsm_ns-->>-main: keystore_size
```

## Sequence diagram of keygen when TF-M is enabled
```mermaid
sequenceDiagram
    participant main
    participant bls_hsm_ns
    participant secure_partition_interface
    participant secure_partition
    participant bls_hsm
    
    main->>+bls_hsm_ns: keygen()
    
    bls_hsm_ns->>+secure_partition_interface: tfm_get_keystore_size()
    secure_partition_interface->>+secure_partition: tfm_ns_interface_dispatch(tfm_get_keystore_size_req_veneer...)
    secure_partition->>+bls_hsm: get_keystore()
    bls_hsm-->>-secure_partition: keystore_size
    secure_partition-->>-secure_partition_interface: keystore_size
    secure_partition_interface-->>-bls_hsm_ns: keystore_size
    
    bls_hsm_ns->>+secure_partition_interface: tfm_secure_keygen()
    secure_partition_interface->>+secure_partition: tfm_ns_interface_dispatch(tfm_secure_keygen_req_veneer...)
    secure_partition->>+bls_hsm: secure_keygen()
    bls_hsm-->>-secure_partition: index
    secure_partition-->>-secure_partition_interface: index
    secure_partition_interface-->>-bls_hsm_ns: index
    
    bls_hsm_ns->>+secure_partition_interface: tfm_get_key()
    secure_partition_interface->>+secure_partition: tfm_ns_interface_dispatch(tfm_get_key_req_veneer...)
    secure_partition->>+bls_hsm: get_key()
    bls_hsm-->>-secure_partition: pk
    secure_partition-->>-secure_partition_interface: pk
    secure_partition_interface-->>-bls_hsm_ns: pk
    
    bls_hsm_ns-->>-main: keystore_size
```
