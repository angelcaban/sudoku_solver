#include <curl/curl.h>
#include <sstream>
#include <iostream>

namespace curl {

  size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

  enum class CurlError {
    SUCCESS,
    ALREADY_READING,
    CURL_ERROR,
  };

  class CurlWrapper {
  private:
    CURL* ctx_;
    bool is_reading_;
    std::stringstream in_stream_;

  public:
    CurlWrapper() {
      is_reading_ = false;
      ctx_ = curl_easy_init();
      curl_easy_setopt(ctx_, CURLOPT_WRITEFUNCTION, ::curl::write_callback);
      curl_easy_setopt(ctx_, CURLOPT_WRITEDATA, reinterpret_cast<void*>(this));

      struct curl_slist* header_chunk = NULL;
      header_chunk = curl_slist_append(header_chunk, "accept: */*");
      header_chunk = curl_slist_append(header_chunk, "accept - encoding: gzip, deflate, br");
      header_chunk = curl_slist_append(header_chunk, "accept - language: en - US, en; q = 0.9");
      header_chunk = curl_slist_append(header_chunk, "cookie: mode = classic; checkMistakes = false; "
                                                     "_ga = GA1.1.16803027.1637287901; sdk_adw = 1; "
                                                     "sdk_analytics = 1; sdk_confirm = 1; __cflb = "
                                                    "02DiuE7hKpaqvCsoqtTrKvfsPpYGyrLgYXXqpouBneX5E; "
                                                    "_ga_LKCCSV4WGG = GS1.1.1662769050.5.1.1662769062.0.0.0");
      header_chunk = curl_slist_append(header_chunk, "dnt: 1");
      header_chunk = curl_slist_append(header_chunk, "referer: https://sudoku.com/evil/");
      header_chunk = curl_slist_append(header_chunk, "sec-ch-ua: \"Chromium\";v=\"104\", "
                                                     "\" Not A;Brand\";v=\"99\", "
                                                     "\"Google Chrome\";v=\"104\"");
      header_chunk = curl_slist_append(header_chunk, "sec-ch-ua-mobile: ?0");
      header_chunk = curl_slist_append(header_chunk, "sec-ch-ua-platform: \"Windows\"");
      header_chunk = curl_slist_append(header_chunk, "sec-fetch-dest: empty");
      header_chunk = curl_slist_append(header_chunk, "sec-fetch-mode: cors");
      header_chunk = curl_slist_append(header_chunk, "sec-fetch-site: same-origin");
      header_chunk = curl_slist_append(header_chunk, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                                                     "AppleWebKit/537.36 (KHTML, like Gecko) "
                                                     "Chrome/104.0.0.0 Safari/537.36");
      header_chunk = curl_slist_append(header_chunk, "x-easy-locale: en");
      header_chunk = curl_slist_append(header_chunk, "x-requested-with: XMLHttpRequest");
      curl_easy_setopt(ctx_, CURLOPT_HTTPHEADER, header_chunk);
    }
    ~CurlWrapper() {
      if (ctx_) {
        curl_easy_cleanup(ctx_);
      }
    }

    CurlError start_read(const char* url) {
      if (is_reading_) {
        return CurlError::ALREADY_READING;
      }

      in_stream_ = std::stringstream{};
      is_reading_ = true;
      curl_easy_setopt(ctx_, CURLOPT_URL, url);
      auto resp = curl_easy_perform(ctx_);
      if (resp != CURLE_OK) {
        std::cerr << "Curl error (" << resp << ") while attempting GET on " << url
          << " -- " << curl_easy_strerror(resp) << "\n";
        return CurlError::CURL_ERROR;
      }

      return CurlError::SUCCESS;
    }

    std::stringstream& stream() {
      return in_stream_;
    }

    friend size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
  };

  size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    CurlWrapper* wrapper = reinterpret_cast<CurlWrapper*>(userdata);
    wrapper->in_stream_.write(ptr, size * nmemb);
    return size * nmemb;
  }
} // namespace curl