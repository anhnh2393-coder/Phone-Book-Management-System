# 5. Class Diagram Specification

## 5.1. Class Details and Descriptions

### 1. Class: PHONEBOOK
* **Description:** Acts as the main controller handling overall system operations, contact/group management, and import/export tasks.
* **Attributes:**
  - `bookId`: int
  - `contacts`: List
  - `groups`: List
* **Main Methods:**
  - `addContact()`: void
  - `searchContact()`: List
  - `updateContact()`: void
  - `deleteContact()`: void
  - `addGroup()`: void
  - `importContact()`: void
  - `exportContact()`: void

### 2. Class: CONTACT
* **Description:** Represents an individual contact entry and tracks personal details along with favorite status.
* **Attributes:**
  - `contactId`: int
  - `fullName`: string
  - `email`: string
  - `address`: string
  - `note`: string
  - `isFavorite`: boolean
* **Main Methods:**
  - `getContactId()`: int
  - `setFavorite()`: void

### 3. Class: PHONENUMBER
* **Description:** Stores phone number details, checks input validity, and handles formatting.
* **Attributes:**
  - `phoneId`: int
  - `contactId`: int
  - `number`: string
  - `type`: string
* **Main Methods:**
  - `validateNumber()`: boolean
  - `updateNumber()`: void
  - `formatNumber()`: string

### 4. Class: GROUP
* **Description:** Manages contact categories, allowing users to organize contacts into distinct groups.
* **Attributes:**
  - `groupId`: int
  - `groupName`: string
  - `description`: string
* **Main Methods:**
  - `create()`: void
  - `update()`: void
  - `delete()`: void

### 5. Class: CONTACTGROUP
* **Description:** Connects contacts and groups together to support many-to-many relationships.
* **Attributes:**
  - `contactId`: int
  - `groupId`: int
* **Main Methods:**
  - `assign()`: void
  - `remove()`: void

---

## 5.2. Class Overview Table

| Class | Attributes | Main Methods | Responsibility |
| :--- | :--- | :--- | :--- |
| **PHONEBOOK** | `bookId`, `contacts`, `groups` | `addContact()`, `searchContact()`, `updateContact()`, `deleteContact()`, `addGroup()`, `importContact()`, `exportContact()` | Central controller handling system operations, contact/group management, and import/export tasks. |
| **CONTACT** | `contactId`, `fullName`, `email`, `address`, `note`, `isFavorite` | `getContactId()`, `setFavorite()` | Represents an individual contact entry and tracks personal details along with favorite status. |
| **PHONENUMBER** | `phoneId`, `contactId`, `number`, `type` | `validateNumber()`, `updateNumber()`, `formatNumber()` | Stores phone number details, checks input validity, and handles formatting. |
| **GROUP** | `groupId`, `groupName`, `description` | `create()`, `update()`, `delete()` | Manages contact categories, allowing users to organize contacts into distinct groups. |
| **CONTACTGROUP** | `contactId`, `groupId` | `assign()`, `remove()` | Connects contacts and groups together to support many-to-many relationships. |

---

## 5.3. Relationship Summary

* **`PHONEBOOK` to `CONTACT` (`1` to `0..*`):** One phone book stores and manages all your contacts.
* **`PHONEBOOK` to `GROUP` (`1` to `0..*`):** One phone book holds all custom groups created to organize people.
* **`CONTACT` to `PHONENUMBER` (`1` to `0..*`):** Each contact can have multiple numbers (like home or work). Deleting a contact automatically removes all associated numbers.
* **`CONTACT` to `GROUP` via `CONTACTGROUP` (`1` to `0..*`):** `CONTACTGROUP` links contacts and groups together so that a single contact can belong to several groups, and a single group can hold multiple contacts.
