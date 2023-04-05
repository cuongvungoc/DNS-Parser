// Function to parse a DNS query and get the name length and label count
void parse_dns_query(const unsigned char *buffer, int query_offset, int *name_length, int *label_count) {
    // Initialize variables
    int i = 0;
    int j = 0;
    int k = 0;
    int is_label_start = 1;
    int is_pointer = 0;
    *name_length = 0;
    *label_count = 0;

    // Parse domain name labels
    while (buffer[query_offset + i] != 0) {
        if (buffer[query_offset + i] >= 192) {
            // Pointer, jump to the offset specified in the next byte
            is_pointer = 1;
            i = buffer[query_offset + i + 1];
            continue;
        } else {
            // Label, count characters and separators
            if (is_label_start) {
                // Start of new label, increment label count
                (*label_count)++;
                is_label_start = 0;
            }
            if (!is_pointer) {
                // Not a pointer, count characters and separators
                (*name_length)++;
            }
        }
        if (is_pointer || buffer[query_offset + i] == '.') {
            // End of label, reset label start flag
            is_label_start = 1;
        }
        i++;
    }
    if (!is_label_start) {
        // Last label not terminated by separator, increment label count
        (*label_count)++;
    }

    // Add length of null terminator to name length
    (*name_length)++;

    return;
}