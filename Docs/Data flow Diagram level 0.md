```mermaid
graph TD
    %% Entity & Process Nodes
    USER[USER]
    CLOUD[CLOUD SYNC SERVICE]
    BACKUP[BACKUP SERVICE]
    SYSTEM((0.0<br/>Phone Book<br/>Management<br/>System))

    %% Data Flows: USER <--> SYSTEM
    USER -->|Add/Edit/Delete/Search Request| SYSTEM
    USER -->|Contact Details| SYSTEM
    SYSTEM -->|Search Results| USER
    SYSTEM -->|Contact Details / Confirmation Message / Error| USER

    %% Data Flows: SYSTEM <--> CLOUD SYNC SERVICE
    CLOUD -->|Remote Contact| SYSTEM
    SYSTEM -->|Upload Updates| CLOUD
    CLOUD -->|Sync Status| SYSTEM
    CLOUD -->|Restore Data| SYSTEM

    %% Data Flows: SYSTEM <--> BACKUP SERVICE
    SYSTEM -->|Backup Request| BACKUP
    BACKUP -->|Backup Data| SYSTEM
