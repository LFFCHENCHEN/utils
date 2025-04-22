#include <stdio.h>
#include <curl/curl.h>

// Progress callback function
static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (ultotal > 0)
    {
        printf("\rUpload progress: %.2f%%", (double)ulnow / ultotal * 100);
        fflush(stdout);
    }
    return 0; // Return 0 to continue the transfer
}

// Function to upload a file using libcurl
void upload_file_with_curl(const char *url, const char *file_path)
{
    CURL *curl;
    CURLcode res;
    FILE *file;

    // Open the file to be uploaded
    file = fopen(file_path, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    // Initialize a CURL session
    curl = curl_easy_init();
    if (curl)
    {
        // Set the URL for the upload
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Specify the file to upload
        curl_easy_setopt(curl, CURLOPT_READDATA, file);

        // Use PUT method for file upload
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Set the progress callback
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress meter

        // Perform the file upload
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            printf("\nUpload completed successfully.\n");
        }

        // Clean up the CURL session
        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Failed to initialize CURL\n");
    }

    // Close the file
    fclose(file);
}

// Example usage
int main()
{
    const char *url = "http://example.com/upload";
    const char *file_path = "example.txt";

    upload_file_with_curl(url, file_path);
    return 0;
}
