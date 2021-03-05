#ifndef HV_FILE_CACHE_H_
#define HV_FILE_CACHE_H_

#include <memory>
#include <map>
#include <string>
#include <mutex>

#include "hbuf.h"
#include "hstring.h"

#define HTTP_HEADER_MAX_LENGTH      1024        // 1K
#define FILE_CACHE_MAX_SIZE         (1 << 30)   // 1G

typedef struct file_cache_s {
    //std::string filepath;
    struct stat st;
    time_t      open_time;
    time_t      stat_time;
    uint32_t    stat_cnt;
    HBuf        buf; // http_header + file_content
    hbuf_t      filebuf;
    hbuf_t      httpbuf;
    char        last_modified[64];
    char        etag[64];
    const char* content_type;

    file_cache_s() {
        stat_cnt = 0;
        content_type = NULL;
    }

    bool if_modified(const char* filepath) {
        time_t mtime = st.st_mtime;
        stat(filepath, &st);
        if (mtime == st.st_mtime) {
            return false;
        }
        return true;
    }

    void resize_buf(int filesize) {
        buf.resize(HTTP_HEADER_MAX_LENGTH + filesize);
        filebuf.base = buf.base + HTTP_HEADER_MAX_LENGTH;
        filebuf.len = filesize;
    }

    void prepend_header(const char* header, int len) {
        if (len > HTTP_HEADER_MAX_LENGTH) return;
        httpbuf.base = filebuf.base - len;
        httpbuf.len = len + filebuf.len;
        memcpy(httpbuf.base, header, len);
    }
} file_cache_t;

typedef std::shared_ptr<file_cache_t>           file_cache_ptr;
// filepath => file_cache_ptr
typedef std::map<std::string, file_cache_ptr>   FileCacheMap;

#define DEFAULT_FILE_STAT_INTERVAL      10 // s
#define DEFAULT_FILE_EXPIRED_TIME       60 // s
class FileCache {
public:
    int file_stat_interval;
    int file_expired_time;
    FileCacheMap    cached_files;
    std::mutex      mutex_;

    FileCache() {
        file_stat_interval = DEFAULT_FILE_STAT_INTERVAL;
        file_expired_time  = DEFAULT_FILE_EXPIRED_TIME;
    }

    file_cache_ptr Open(const char* filepath, bool need_read = true, void* ctx = NULL);
    bool Close(const char* filepath);
    bool Close(const file_cache_ptr& fc);
    void RemoveExpiredFileCache();

protected:
    file_cache_ptr Get(const char* filepath);
};

#endif // HV_FILE_CACHE_H_
