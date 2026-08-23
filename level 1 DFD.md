flowchart TD
    %% Định nghĩa các thực thể bên ngoài (External Entities)
    USER[USER]
    CLOUD[CLOUD SYNC SERVICE]
    BACKUP[BACKUP SERVICE]

    %% Định nghĩa các tiến trình (Processes)
    P1((1.0 Manage Contacts))
    P2((2.0 Search Contacts))
    P0((0.0 Phone Book Management System))
    P3((3.0 Perform Cloud Sync))
    P4((4.0 Manage Backups))
    
    %% Định nghĩa Kho dữ liệu (Data Stores)
    D1[(D1 Contact Database)]

    %% --- CÁC LUỒNG DỮ LIỆU (DATA FLOWS) ---

    %% Nhóm 1.0 Manage Contacts
    USER -->|Add/Edit/Delete Request| P1
    USER -->|Contact Details| P1
    P1 -->|Confirmation Message/Error| USER
    P1 -->|Contact Details| USER
    P1 -->|Write/Update Contact Data| P0

    %% Nhóm 2.0 Search Contacts
    USER -->|Search Request| P2
    P2 -->|Search Results| USER
    P0 -->|Filtered Contact Details| P2

    %% Nhóm 3.0 Perform Cloud Sync
    P0 -->|All Contact Data| P3
    P3 -->|Upload Updates| CLOUD
    CLOUD -->|Remote Contact| P3
    CLOUD -->|Sync Status| P3
    CLOUD -->|Restore Data| P3
    P3 -->|Write Restored Data| D1

    %% Nhóm 4.0 Manage Backups
    D1 -->|All Contact Database| P4
    P4 -->|Backup Request| BACKUP
    BACKUP -->|Backup Data| P4
