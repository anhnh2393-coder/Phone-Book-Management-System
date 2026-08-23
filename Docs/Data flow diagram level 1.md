```mermaid
graph TD
    %% External Entities
    USER[USER]
    CLOUD[CLOUD SYNC SERVICE]
    BACKUP[BACKUP SERVICE]

    %% Processes
    P1((1.0<br/>Manage<br/>Contacts))
    P2((2.0<br/>Search<br/>Contacts))
    P3((3.0<br/>Perform Cloud<br/>Sync))
    P4((4.0<br/>Manage<br/>Backups))

    %% Data Store
    D1[(D1 Contact Database)]

    %% Connections for USER
    USER -->|Add/Edit/Delete Request| P1
    USER -->|Contact Details| P1
    P1 -->|Confirmation Message/Error| USER

    USER -->|Search Request| P2
    P2 -->|Search Results| USER

    USER -->|Backup/Restore Request| P4

    %% Connections for Data Store (D1)
    P1 -->|Write/Update Contact Data| D1
    D1 -->|Filtered Contact Details| P2
    P3 -->|All Contact Data| D1
    P3 -->|Write Restored Data| D1
    P4 -->|Database Backup Data| D1
    D1 -->|Restore Data| P4

    %% Connections for CLOUD SYNC SERVICE
    P3 -->|Upload Updates| CLOUD
    CLOUD -->|Remote Contact| P3
    CLOUD -->|Sync Status| P3
    CLOUD -->|Restore Data| P3

    %% Connections for BACKUP SERVICE
    P4 -->|Backup Request| BACKUP
    BACKUP -->|Backup Data| P4
