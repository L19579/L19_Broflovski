# L19_Broflovski

## Purpose: Educational

AMM subprogram account aggregator, written as an intro to C, CMake and SQL.


![example_img](https://github.com/L19579/L19_Broflovski/blob/master/imgs/e.png)
---

## Instructions

#### Environment 
  Linux (Fedora 29), x86_64 ; GCC.  

#### Required libraries 
  libcurl, openssl, postgresql and json-c are required.
  For RHEL/CentOS/Fedora (replace `dnf` with `yum` if it's not installed)
  ```bash
    $ sudo dnf update
    $ sudo dnf install openssl-devel libcurl-devel postgresql-devel
  ```
  
  Follow [these](https://github.com/json-c/json-c) instructions for json-c install

#### PostgreSQL
  See [postgresql tutorial] for SQL basics.

  Database credentials and target assigned to `DB_CREDS_AND_TARGET` in `broflovski.c`  
  must be valid before build.

#### Build and run example.c
  ```bash
  $ git clone https://github.com/L19579/L19_Broflovski.git
  $ cd L19_Broflovski
  $ mkdir build; cd build
  $ cmake ..
  $ make
  $ ./example/example
  ```

#### Configuration
  Add target AMM parent program to `config.toml`
  ```toml
    [[swap]]
    name = "saros" # must be unique
    key = "SSwapUtytfBdBn1b9NUGG6foMVPtcWgpRU32HToDUZr"
    schema_type = 3
    ready = true 
  ```

  `schema_type` points to SwapType enum variant reflecting grouped solscan account labels  
  the program will look for. See `amm_keys` .h and .c files.
  ```c
  switch(SwapType){
    /* snip */
    case Saros:
      strcpy(amm_data->amm_schema, "saros_type"); // reuse ok for AMMs w/ matching account labels
      amm_data->n_account_keys = 10;  
      ignore = (uint8_t *[3]){2, 3, 6};           // foreign user account indexes
      amm_data->n_ignore = 3;                     // n of foreign user accounts
      account_keys = (char*[10]){
        "Swap",
        "Authority",
        "User Transfer Authority",
        "Source",
        "Swap Source",
        "Swap Destination",
        "Destination",
        "Pool Mint",
        "Pool Fee",
        "TOKEN_PROGRAM_ID",
      };
      break;
    /* snip */
  };
  ```
  
 `example/` directory structure can be used for custom CMake executable, modify `CMakeLists.txt`  
 for new target.

---

## Notes
On going development. TODOs are listed in .c files.

Subject to Solscan's rate limits.  

---

## License

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

See terms: [License](https://github.com/L19579/L19_Broflovski/blob/master/LICENSE)
 
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
