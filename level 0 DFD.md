flowchart LR
    %% Định nghĩa các thực thể bên ngoài (External Entities)
    USER[USER]
    CLOUD[CLOUD SYNC SERVICE]
    BACKUP[BACKUP SERVICE]

    %% Định nghĩa Tiến trình chính (0-level Process)
    P0((0.0 Phone Book Management System))

    %% --- CÁC LUỒNG DỮ LIỆU ---

    %% Giữa USER và Hệ thống
    USER -->|Add/Edit/Delete/Search Request| P0
    USER -->|Contact Details| P0
    P0 -->|Search Results| USER
    P0 -->|Contact Details/ Confirmation Message/ Error| USER
    
    %% Giữa Hệ thống và CLOUD SYNC SERVICE
    CLOUD -->|Remote Contact| P0
    P0 -->|Upload Updates| CLOUD
    CLOUD -->|Sync Status| P0
    CLOUD -->|Restore Data| P0
    
    %% Giữa Hệ thống và BACKUP SERVICE
    P0 -->|Backup Request| BACKUP
    BACKUP -->|Backup Data| P0
