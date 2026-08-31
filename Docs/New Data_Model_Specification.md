# 6. DATA MODEL SPECIFICATION

## 6.1 Overview

The Phone Book Management System uses a relational data model to store and manage user accounts, address books, contacts, phone numbers, email addresses, addresses, groups, tags, favorites, notes, reminders, custom fields, and import history.

The data model is designed to ensure data consistency, reduce data redundancy, and support the main functions of the system such as creating and managing contacts, organizing contacts into groups and tags, marking contacts as favorites, adding reminders and notes, and importing contact information.

The main entities in the system are:

- USER
- USER_SETTING
- ADDRESS_BOOK
- CONTACT
- PHONE_NUMBER
- EMAIL_ADDRESS
- ADDRESS
- GROUP
- CONTACT_GROUP_MEMBER
- FAVORITE
- NOTE
- REMINDER
- CUSTOM_FIELD
- TAG
- CONTACT_TAG
- IMPORT_LOG

---

## 6.2 Entity Specifications

### 6.2.1 USER

**Description:**

The USER entity stores information about users who access and use the Phone Book Management System. Each user can manage one or more address books, configure personal settings, mark contacts as favorites, and import contact information.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| UserID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the user |
| FullName | VARCHAR(100) | | NOT NULL | Full name of the user |
| Email | VARCHAR(150) | UNIQUE | NOT NULL | Email address used for the account |
| PasswordHash | VARCHAR(255) | | NOT NULL | Encrypted user password |
| Phone | VARCHAR(20) | | NULL | User's phone number |
| CreatedAt | DATETIME | | NOT NULL | Date and time when the account was created |

**Primary Key:** UserID

**Relationships:**

- One USER can own many ADDRESS_BOOK records.
- One USER has one USER_SETTING record.
- One USER can have many FAVORITE records.
- One USER can have many IMPORT_LOG records.

---

### 6.2.2 USER_SETTING

**Description:**

The USER_SETTING entity stores personal preferences and system settings for each user.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| UserID | INT | PK, FK | NOT NULL | Identifier of the user |
| Theme | VARCHAR(20) | | DEFAULT 'Light' | Interface theme |
| Language | VARCHAR(20) | | DEFAULT 'Vietnamese' | Preferred system language |
| DateFormat | VARCHAR(20) | | NULL | Preferred date format |
| TimeZone | VARCHAR(50) | | NULL | User's time zone |
| BackupReminder | BOOLEAN | | DEFAULT TRUE | Indicates whether backup reminders are enabled |

**Primary Key:** UserID

**Foreign Key:** UserID references USER(UserID)

**Relationship:**

USER 1:1 USER_SETTING

**Business Rule:**

Each user can have only one USER_SETTING record.

---

### 6.2.3 ADDRESS_BOOK

**Description:**

The ADDRESS_BOOK entity represents a phone book owned by a user. A user can create multiple address books to organize contacts.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| BookID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the address book |
| UserID | INT | FK | NOT NULL | Identifier of the owner |
| BookName | VARCHAR(100) | | NOT NULL | Name of the address book |
| Description | TEXT | | NULL | Description of the address book |
| CreatedAt | DATETIME | | NOT NULL | Creation date and time |
| UpdatedAt | DATETIME | | NULL | Last update date and time |

**Primary Key:** BookID

**Foreign Key:** UserID references USER(UserID)

**Relationships:**

- USER 1:N ADDRESS_BOOK
- ADDRESS_BOOK 1:N CONTACT
- ADDRESS_BOOK 1:N GROUP
- ADDRESS_BOOK 1:N TAG

**Business Rule:**

Each address book belongs to exactly one user.

---

### 6.2.4 CONTACT

**Description:**

The CONTACT entity stores the basic information of a person or organization in an address book.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| ContactID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the contact |
| BookID | INT | FK | NOT NULL | Identifier of the address book |
| FirstName | VARCHAR(50) | | NOT NULL | First name of the contact |
| LastName | VARCHAR(50) | | NOT NULL | Last name of the contact |
| Nickname | VARCHAR(50) | | NULL | Nickname of the contact |
| Company | VARCHAR(100) | | NULL | Company where the contact works |
| JobTitle | VARCHAR(100) | | NULL | Job title of the contact |
| CreatedAt | DATETIME | | NOT NULL | Creation date and time |
| UpdatedAt | DATETIME | | NULL | Last update date and time |

**Primary Key:** ContactID

**Foreign Key:** BookID references ADDRESS_BOOK(BookID)

**Relationships:**

- ADDRESS_BOOK 1:N CONTACT
- CONTACT 1:N PHONE_NUMBER
- CONTACT 1:N EMAIL_ADDRESS
- CONTACT 1:N ADDRESS
- CONTACT 1:N NOTE
- CONTACT 1:N REMINDER
- CONTACT 1:N CUSTOM_FIELD
- CONTACT 1:N FAVORITE
- CONTACT 1:N CONTACT_GROUP_MEMBER
- CONTACT 1:N CONTACT_TAG

**Business Rule:**

Each contact must belong to one address book.

---

### 6.2.5 PHONE_NUMBER

**Description:**

The PHONE_NUMBER entity stores phone numbers associated with a contact. A contact can have multiple phone numbers, such as mobile, home, or work numbers.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| PhoneID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the phone number |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| PhoneNumber | VARCHAR(20) | | NOT NULL | Phone number |
| PhoneType | VARCHAR(20) | | NOT NULL | Type of phone number |
| IsPrimary | BOOLEAN | | DEFAULT FALSE | Indicates whether the number is the primary number |

**Primary Key:** PhoneID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N PHONE_NUMBER

---

### 6.2.6 EMAIL_ADDRESS

**Description:**

The EMAIL_ADDRESS entity stores email addresses associated with a contact.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| EmailID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the email |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| EmailAddress | VARCHAR(150) | | NOT NULL | Email address |
| EmailType | VARCHAR(20) | | NOT NULL | Type of email address |
| IsPrimary | BOOLEAN | | DEFAULT FALSE | Indicates whether the email is primary |

**Primary Key:** EmailID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N EMAIL_ADDRESS

---

### 6.2.7 ADDRESS

**Description:**

The ADDRESS entity stores physical addresses associated with a contact.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| AddressID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the address |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| AddressType | VARCHAR(20) | | NOT NULL | Type of address |
| Street | VARCHAR(150) | | NULL | Street address |
| City | VARCHAR(100) | | NULL | City |
| State | VARCHAR(100) | | NULL | State or province |
| PostalCode | VARCHAR(20) | | NULL | Postal code |
| Country | VARCHAR(100) | | NULL | Country |
| IsPrimary | BOOLEAN | | DEFAULT FALSE | Indicates whether the address is primary |

**Primary Key:** AddressID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N ADDRESS

---

### 6.2.8 GROUP

**Description:**

The GROUP entity is used to organize contacts into logical groups such as Family, Friends, Work, or School.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| GroupID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the group |
| BookID | INT | FK | NOT NULL | Identifier of the address book |
| GroupName | VARCHAR(100) | | NOT NULL | Name of the group |
| Description | TEXT | | NULL | Description of the group |

**Primary Key:** GroupID

**Foreign Key:** BookID references ADDRESS_BOOK(BookID)

**Relationships:**

- ADDRESS_BOOK 1:N GROUP
- GROUP 1:N CONTACT_GROUP_MEMBER

---

### 6.2.9 CONTACT_GROUP_MEMBER

**Description:**

The CONTACT_GROUP_MEMBER entity is an associative entity used to manage the many-to-many relationship between CONTACT and GROUP.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| ContactID | INT | PK, FK | NOT NULL | Identifier of the contact |
| GroupID | INT | PK, FK | NOT NULL | Identifier of the group |

**Primary Key:**

(ContactID, GroupID)

**Foreign Keys:**

- ContactID references CONTACT(ContactID)
- GroupID references GROUP(GroupID)

**Relationships:**

- CONTACT 1:N CONTACT_GROUP_MEMBER
- GROUP 1:N CONTACT_GROUP_MEMBER

**Business Rule:**

A contact can belong to multiple groups, and each group can contain multiple contacts.

Therefore:

CONTACT N:N GROUP

through CONTACT_GROUP_MEMBER.

---

### 6.2.10 FAVORITE

**Description:**

The FAVORITE entity stores the contacts that users have marked as favorites.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| UserID | INT | PK, FK | NOT NULL | Identifier of the user |
| ContactID | INT | PK, FK | NOT NULL | Identifier of the contact |
| AddedAt | DATETIME | | NOT NULL | Time when the contact was added to favorites |

**Primary Key:**

(UserID, ContactID)

**Foreign Keys:**

- UserID references USER(UserID)
- ContactID references CONTACT(ContactID)

**Relationships:**

- USER 1:N FAVORITE
- CONTACT 1:N FAVORITE

Therefore:

USER N:N CONTACT

through FAVORITE.

**Business Rule:**

A user cannot add the same contact to favorites more than once.

---

### 6.2.11 NOTE

**Description:**

The NOTE entity stores additional notes related to a contact.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| NoteID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the note |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| NoteTitle | VARCHAR(150) | | NOT NULL | Title of the note |
| NoteContent | TEXT | | NOT NULL | Content of the note |
| CreatedAt | DATETIME | | NOT NULL | Creation date and time |
| UpdatedAt | DATETIME | | NULL | Last update date and time |

**Primary Key:** NoteID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N NOTE

---

### 6.2.12 REMINDER

**Description:**

The REMINDER entity stores reminders related to contacts, such as birthdays, meetings, or important events.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| ReminderID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the reminder |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| Title | VARCHAR(150) | | NOT NULL | Reminder title |
| ReminderDate | DATETIME | | NOT NULL | Date and time of the reminder |
| Description | TEXT | | NULL | Reminder description |
| IsCompleted | BOOLEAN | | DEFAULT FALSE | Indicates whether the reminder is completed |
| CreatedAt | DATETIME | | NOT NULL | Creation date and time |

**Primary Key:** ReminderID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N REMINDER

---

### 6.2.13 CUSTOM_FIELD

**Description:**

The CUSTOM_FIELD entity allows additional information to be stored for a contact when the information does not belong to the standard contact attributes.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| FieldID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the custom field |
| ContactID | INT | FK | NOT NULL | Identifier of the contact |
| FieldName | VARCHAR(100) | | NOT NULL | Name of the custom field |
| FieldValue | TEXT | | NULL | Value of the custom field |
| FieldType | VARCHAR(30) | | NULL | Type of the custom field |

**Primary Key:** FieldID

**Foreign Key:** ContactID references CONTACT(ContactID)

**Relationship:**

CONTACT 1:N CUSTOM_FIELD

---

### 6.2.14 TAG

**Description:**

The TAG entity stores labels used to classify and organize contacts.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| TagID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the tag |
| BookID | INT | FK | NOT NULL | Identifier of the address book |
| TagName | VARCHAR(50) | | NOT NULL | Name of the tag |
| Description | TEXT | | NULL | Description of the tag |

**Primary Key:** TagID

**Foreign Key:** BookID references ADDRESS_BOOK(BookID)

**Relationships:**

- ADDRESS_BOOK 1:N TAG
- TAG 1:N CONTACT_TAG

---

### 6.2.15 CONTACT_TAG

**Description:**

The CONTACT_TAG entity is an associative entity used to manage the many-to-many relationship between CONTACT and TAG.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| ContactID | INT | PK, FK | NOT NULL | Identifier of the contact |
| TagID | INT | PK, FK | NOT NULL | Identifier of the tag |

**Primary Key:**

(ContactID, TagID)

**Foreign Keys:**

- ContactID references CONTACT(ContactID)
- TagID references TAG(TagID)

**Relationships:**

- CONTACT 1:N CONTACT_TAG
- TAG 1:N CONTACT_TAG

Therefore:

CONTACT N:N TAG

through CONTACT_TAG.

---

### 6.2.16 IMPORT_LOG

**Description:**

The IMPORT_LOG entity stores the history of contact data imported by users into the system.

**Attributes:**

| Attribute | Data Type | Key | Constraint | Description |
|---|---|---|---|---|
| ImportID | INT | PK | NOT NULL, AUTO_INCREMENT | Unique identifier of the import operation |
| UserID | INT | FK | NOT NULL | Identifier of the user performing the import |
| FileName | VARCHAR(255) | | NOT NULL | Name of the imported file |
| ImportDate | DATETIME | | NOT NULL | Date and time of the import |
| TotalRecords | INT | | DEFAULT 0 | Total number of imported records |
| SuccessRecords | INT | | DEFAULT 0 | Number of successfully imported records |
| FailedRecords | INT | | DEFAULT 0 | Number of failed records |

**Primary Key:** ImportID

**Foreign Key:** UserID references USER(UserID)

**Relationship:**

USER 1:N IMPORT_LOG

**Business Rule:**

Each import operation creates one IMPORT_LOG record. A user can perform multiple import operations.

---

## 6.3 Relationship Specification

The relationships among entities in the Phone Book Management System are defined as follows:

| No. | Relationship | Cardinality | Description |
|---|---|---|---|
| 1 | USER – USER_SETTING | 1:1 | Each user has one personal setting record |
| 2 | USER – ADDRESS_BOOK | 1:N | A user can own multiple address books |
| 3 | USER – FAVORITE | 1:N | A user can have many favorite records |
| 4 | USER – IMPORT_LOG | 1:N | A user can perform many import operations |
| 5 | ADDRESS_BOOK – CONTACT | 1:N | An address book can contain many contacts |
| 6 | ADDRESS_BOOK – GROUP | 1:N | An address book can contain many groups |
| 7 | ADDRESS_BOOK – TAG | 1:N | An address book can contain many tags |
| 8 | CONTACT – PHONE_NUMBER | 1:N | A contact can have multiple phone numbers |
| 9 | CONTACT – EMAIL_ADDRESS | 1:N | A contact can have multiple email addresses |
| 10 | CONTACT – ADDRESS | 1:N | A contact can have multiple addresses |
| 11 | CONTACT – NOTE | 1:N | A contact can have multiple notes |
| 12 | CONTACT – REMINDER | 1:N | A contact can have multiple reminders |
| 13 | CONTACT – CUSTOM_FIELD | 1:N | A contact can have multiple custom fields |
| 14 | CONTACT – GROUP | N:N | Managed through CONTACT_GROUP_MEMBER |
| 15 | CONTACT – TAG | N:N | Managed through CONTACT_TAG |
| 16 | USER – CONTACT | N:N | Managed through FAVORITE |

---

## 6.4 Primary Key and Foreign Key Specification

### Primary Keys

- USER: UserID
- USER_SETTING: UserID
- ADDRESS_BOOK: BookID
- CONTACT: ContactID
- PHONE_NUMBER: PhoneID
- EMAIL_ADDRESS: EmailID
- ADDRESS: AddressID
- GROUP: GroupID
- CONTACT_GROUP_MEMBER: (ContactID, GroupID)
- FAVORITE: (UserID, ContactID)
- NOTE: NoteID
- REMINDER: ReminderID
- CUSTOM_FIELD: FieldID
- TAG: TagID
- CONTACT_TAG: (ContactID, TagID)
- IMPORT_LOG: ImportID

### Foreign Keys

- USER_SETTING.UserID → USER.UserID
- ADDRESS_BOOK.UserID → USER.UserID
- CONTACT.BookID → ADDRESS_BOOK.BookID
- PHONE_NUMBER.ContactID → CONTACT.ContactID
- EMAIL_ADDRESS.ContactID → CONTACT.ContactID
- ADDRESS.ContactID → CONTACT.ContactID
- GROUP.BookID → ADDRESS_BOOK.BookID
- CONTACT_GROUP_MEMBER.ContactID → CONTACT.ContactID
- CONTACT_GROUP_MEMBER.GroupID → GROUP.GroupID
- FAVORITE.UserID → USER.UserID
- FAVORITE.ContactID → CONTACT.ContactID
- NOTE.ContactID → CONTACT.ContactID
- REMINDER.ContactID → CONTACT.ContactID
- CUSTOM_FIELD.ContactID → CONTACT.ContactID
- TAG.BookID → ADDRESS_BOOK.BookID
- CONTACT_TAG.ContactID → CONTACT.ContactID
- CONTACT_TAG.TagID → TAG.TagID
- IMPORT_LOG.UserID → USER.UserID

---

## 6.5 Business Rules

1. Each USER must have a unique UserID and Email.
2. A USER can own multiple ADDRESS_BOOK records.
3. Each ADDRESS_BOOK belongs to exactly one USER.
4. Each CONTACT belongs to exactly one ADDRESS_BOOK.
5. A CONTACT can have multiple PHONE_NUMBER records.
6. A CONTACT can have multiple EMAIL_ADDRESS records.
7. A CONTACT can have multiple ADDRESS records.
8. A CONTACT can have multiple NOTE records.
9. A CONTACT can have multiple REMINDER records.
10. A CONTACT can have multiple CUSTOM_FIELD records.
11. A GROUP belongs to one ADDRESS_BOOK and can contain multiple CONTACT records.
12. A CONTACT can belong to multiple GROUP records through CONTACT_GROUP_MEMBER.
13. A TAG belongs to one ADDRESS_BOOK and can be assigned to multiple CONTACT records.
14. A CONTACT can have multiple TAG records through CONTACT_TAG.
15. A USER can mark multiple CONTACT records as favorites.
16. The combination of UserID and ContactID in FAVORITE must be unique.
17. The combination of ContactID and GroupID in CONTACT_GROUP_MEMBER must be unique.
18. The combination of ContactID and TagID in CONTACT_TAG must be unique.
19. Each USER has only one USER_SETTING record.
20. Each import operation performed by a USER is recorded in IMPORT_LOG.
21. Deleting a CONTACT should also remove its dependent PHONE_NUMBER, EMAIL_ADDRESS, ADDRESS, NOTE, REMINDER, CUSTOM_FIELD, FAVORITE, CONTACT_GROUP_MEMBER, and CONTACT_TAG records to maintain referential integrity.
22. A CONTACT cannot exist without a valid ADDRESS_BOOK.
23. A GROUP and TAG cannot exist without a valid ADDRESS_BOOK.
24. A FAVORITE record cannot exist without a valid USER and CONTACT.
25. All foreign key values must reference existing records in their corresponding parent entities.
