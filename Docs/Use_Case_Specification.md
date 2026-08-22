# Use Case Specification

---

# 1. Use Case Diagram

## 1.1 Actor List

| No. | Actor | Description |
|---|---|---|
| 1 | User | A person who uses the system to manage contact information. |

## 1.2 Use Case List

| No. | Use Case | Description | Actor |
|---|---|---|---|
| 1 | Add Contact | Add a new contact to the phone book. | User |
| 2 | View Contact List | Display all available contacts. | User |
| 3 | Search Contact | Find a contact by name or phone number. | User |
| 4 | View Contact Details | Display detailed information about a selected contact. | User |
| 5 | Update Contact | Modify existing contact information. | User |
| 6 | Delete Contact | Remove a contact from the system. | User |

---

# 2. Use Case Specifications

## UC01 - Add Contact

| Item | Description |
|---|---|
| Use Case ID | UC01 |
| Use Case Name | Add Contact |
| Primary Actor | User |
| Description | The user adds a new contact to the phone book. |
| Preconditions | The system is running. |
| Main Flow | 1. User selects Add Contact. 2. System displays the contact form. 3. User enters contact information. 4. User confirms. 5. System validates and saves the contact. |
| Postconditions | A new contact is stored successfully. |

## UC02 - View Contact List

| Item | Description |
|---|---|
| Use Case ID | UC02 |
| Use Case Name | View Contact List |
| Primary Actor | User |
| Description | The user views all contacts stored in the system. |
| Preconditions | The system contains contact data or an empty contact list. |
| Main Flow | 1. User selects View Contact List. 2. System retrieves contacts. 3. System displays the list. |
| Postconditions | The contact list is displayed. |

## UC03 - Search Contact

| Item | Description |
|---|---|
| Use Case ID | UC03 |
| Use Case Name | Search Contact |
| Primary Actor | User |
| Description | The user searches for a contact by name or phone number. |
| Preconditions | The system is running. |
| Main Flow | 1. User selects Search. 2. User enters a keyword. 3. System searches the database. 4. System displays matching results. |
| Alternative Flow | If no contact is found, the system displays a notification. |
| Postconditions | Matching contacts or a not-found message are displayed. |

## UC04 - View Contact Details

| Item | Description |
|---|---|
| Use Case ID | UC04 |
| Use Case Name | View Contact Details |
| Primary Actor | User |
| Description | The user views detailed information about a selected contact. |
| Preconditions | The selected contact exists. |
| Main Flow | 1. User selects a contact. 2. System retrieves the information. 3. System displays the contact details. |
| Postconditions | Contact details are displayed. |

## UC05 - Update Contact

| Item | Description |
|---|---|
| Use Case ID | UC05 |
| Use Case Name | Update Contact |
| Primary Actor | User |
| Description | The user modifies information of an existing contact. |
| Preconditions | The selected contact exists. |
| Main Flow | 1. User selects a contact. 2. User selects Update. 3. System displays current information. 4. User edits the information. 5. User confirms. 6. System validates and updates the contact. |
| Postconditions | The contact information is updated. |

## UC06 - Delete Contact

| Item | Description |
|---|---|
| Use Case ID | UC06 |
| Use Case Name | Delete Contact |
| Primary Actor | User |
| Description | The user deletes an existing contact. |
| Preconditions | The selected contact exists. |
| Main Flow | 1. User selects a contact. 2. User selects Delete. 3. System requests confirmation. 4. User confirms deletion. 5. System removes the contact. |
| Alternative Flow | The user cancels the deletion. |
| Postconditions | The selected contact is removed if confirmed. |
