
This repository contains a C++ implementation of a file system server with user, directory, and file operations. It supports authentication, sessions, and basic file system management, accessible via JSON-based requests.

---

## Prerequisites

- Linux or macOS environment
- GCC >= 11 (for C++17 support)
- `make` (optional, for build scripts)
- Libraries:
  - `pthread`
  - `openssl` (`libcrypto`)
- C++17 standard

---

## Build Instructions

1. Clone or download the repository:

```bash
git clone <rhttps://github.com/zainabtouseef/DSA-Project1.git>
cd DSAProject1
```

2. Compile all source files:

```bash
g++ -std=c++17 source/*.cpp -I source/include -lpthread -lcrypto -o ofs_server
```

- `-std=c++17` : Enable C++17 standard
- `-I source/include` : Include path for header files
- `-lpthread` : Thread support
- `-lcrypto` : OpenSSL cryptography library

3. Ensure compilation produces `ofs_server` executable.

---

## Running the Server

Run the compiled server:

```bash
./ofs_server
```

You should see output similar to:

```
[INFO] Configuration loaded successfully
[INFO] Core components initialized successfully
[INFO] No existing FS or failed to load: File not found
[DEBUG create_user] stored user 'admin' password_plain = 'admin123'
[INFO] Admin user created.
[INFO] Server running on port 1010
```

The server listens on port **1010** by default.

---

## Testing the Server

The server uses **JSON-based requests**. You can test using `curl`, Postman, or any HTTP/JSON client.

### 1. Admin Login

```json
{"operation":"user_login","request_id":"req_login_001","payload":{"username":"admin","password":"admin123"}}
```

**Example `curl` request**:

```bash
curl -X POST http://127.0.0.1:1010 \
     -H "Content-Type: application/json" \
     -d '{"operation":"user_login","request_id":"req_login_001","payload":{"username":"admin","password":"admin123"}}'
```

Expected response:

```json
{
  "status": "success",
  "data": {"session_id": "<generated_session_id>"},
  "code": 0,
  "operation": "user_login",
  "request_id": "req_login_001"
}
```

---

### 2. Create User

```json
{
  "operation": "user_create",
  "request_id": "req_create_001",
  "session_id": "<admin_session_id>",
  "username": "alice",
  "password_hash": "alice123",
  "role": 1
}
```

---

### 3. List Users

```json
{
  "operation": "user_list",
  "request_id": "req_list_001",
  "session_id": "<admin_session_id>"
}
```

Expected response includes all users with username, role, and metadata.

---

### 4. Directory Operations

- **Create Directory**:

```json
{"operation":"dir_create","request_id":"req_dir_create","path":"/mydir"}
```

- **Delete Directory**:

```json
{"operation":"dir_delete","request_id":"req_dir_delete","path":"/mydir"}
```

- **List Directory**:

```json
{"operation":"dir_list","request_id":"req_dir_list","path":"/"}
```

---

### 5. File Operations

- **Create File**:

```json
{"operation":"file_create","request_id":"req_file_create","path":"/myfile.txt","size":1024}
```

- **Read File**:

```json
{"operation":"file_read","request_id":"req_file_read","path":"/myfile.txt"}
```

- **Edit File**:

```json
{"operation":"file_edit","request_id":"req_file_edit","path":"/myfile.txt","index":0,"data":"Hello World"}
```

- **Delete File**:

```json
{"operation":"file_delete","request_id":"req_file_delete","path":"/myfile.txt"}
```

---

### 6. Sessions

- `user_login` generates a `session_id`.
- Pass `session_id` for any authenticated operation.
- `user_logout` invalidates the session:

```json
{"operation":"user_logout","request_id":"req_logout_001","session_id":"<session_id>"}
```

---

## Debugging

- All debug messages are printed to console.
- Use `[DEBUG]` logs to verify user authentication, file operations, and directory structure.
- `dump_users()` in `UserManager` prints all stored users for testing.

---

## Notes

- Passwords are stored in plain text (for testing only). Hashing can be enabled later.
- JSON keys are case-sensitive.
- Server currently uses simple in-memory storage and file system simulation.

---
