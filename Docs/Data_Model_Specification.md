# 6. Data Model

The database of the Phone Book Management System includes four main tables: CONTACT, PHONE_NUMBER, GROUP, and CONTACT_GROUP.

## 6.1 Logical Database Schema

The system uses the following tables:

- CONTACT: stores information about contacts.
- PHONE_NUMBER: stores phone numbers of contacts.
- GROUP: stores information about contact groups.
- CONTACT_GROUP: connects contacts with groups.

## 6.2 Table Definitions

| Table | Fields | Description |
|---|---|---|
| CONTACT | contact_id (PK), full_name, address, note, is_favorite | Stores contact information. |
| PHONE_NUMBER | phone_id (PK), contact_id (FK), phone_number, phone_type | Stores phone numbers of contacts. |
| GROUP | group_id (PK), group_name, description | Stores contact group information. |
| CONTACT_GROUP | contact_id (PK, FK), group_id (PK, FK) | Connects contacts and groups. |

## 6.3 Primary Keys and Foreign Keys

### Primary Keys

- contact_id is the primary key of CONTACT.
- phone_id is the primary key of PHONE_NUMBER.
- group_id is the primary key of GROUP.
- contact_id and group_id together form the primary key of CONTACT_GROUP.

### Foreign Keys

- PHONE_NUMBER.contact_id references CONTACT.contact_id.
- CONTACT_GROUP.contact_id references CONTACT.contact_id.
- CONTACT_GROUP.group_id references GROUP.group_id.

## 6.4 Relationships

- One CONTACT can have many PHONE_NUMBER records.
- One CONTACT can have many CONTACT_GROUP records.
- One GROUP can have many CONTACT_GROUP records.
- CONTACT and GROUP have a many-to-many relationship through CONTACT_GROUP.
