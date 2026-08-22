import java.util.ArrayList;
import java.util.List;

class PhoneNumber {
    private int phoneId;
    private int contactId;
    private String number;
    private String type;

    public PhoneNumber(int phoneId, int contactId, String number, String type) {
        this.phoneId = phoneId;
        this.contactId = contactId;
        this.number = number;
        this.type = type;
    }

    public int getPhoneId() { return phoneId; }
    public void setPhoneId(int phoneId) { this.phoneId = phoneId; }

    public int getContactId() { return contactId; }
    public void setContactId(int contactId) { this.contactId = contactId; }

    public String getNumber() { return number; }
    public void setNumber(String number) { this.number = number; }

    public String getType() { return type; }
    public void setType(String type) { this.type = type; }

    public boolean validateNumber() {
        return this.number != null && this.number.matches("\\d{10,11}");
    }

    public void updateNumber(String newNumber, String newType) {
        this.number = newNumber;
        this.type = newType;
    }

    public String formatNumber() {
        if (number != null && number.startsWith("0")) {
            return "+84" + number.substring(1);
        }
        return number;
    }
}

class Contact {
    private int contactId;
    private String fullName;
    private String email;
    private String address;
    private String note;
    private boolean isFavorite;
    
    private List<PhoneNumber> phoneNumbers;

    public Contact(int contactId, String fullName, String email, String address, String note, boolean isFavorite) {
        this.contactId = contactId;
        this.fullName = fullName;
        this.email = email;
        this.address = address;
        this.note = note;
        this.isFavorite = isFavorite;
        this.phoneNumbers = new ArrayList<>();
    }

    public int getContactId() { return contactId; }
    
    public void setFavorite(boolean isFavorite) { this.isFavorite = isFavorite; }
    public boolean isFavorite() { return isFavorite; }

    public String getFullName() { return fullName; }
    public void setFullName(String fullName) { this.fullName = fullName; }

    public String getEmail() { return email; }
    public void setEmail(String email) { this.email = email; }

    public String getAddress() { return address; }
    public void setAddress(String address) { this.address = address; }

    public String getNote() { return note; }
    public void setNote(String note) { this.note = note; }

    public List<PhoneNumber> getPhoneNumbers() { return phoneNumbers; }

    public void addPhoneNumber(PhoneNumber phoneNumber) {
        this.phoneNumbers.add(phoneNumber);
    }
}

class Group {
    private int groupId;
    private String groupName;
    private String description;

    public Group(int groupId, String groupName, String description) {
        this.groupId = groupId;
        this.groupName = groupName;
        this.description = description;
    }

    public int getGroupId() { return groupId; }
    public void setGroupId(int groupId) { this.groupId = groupId; }

    public String getGroupName() { return groupName; }
    public void setGroupName(String groupName) { this.groupName = groupName; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public void create() {
        System.out.println("Tạo nhóm mới: " + groupName);
    }

    public void update(String groupName, String description) {
        this.groupName = groupName;
        this.description = description;
    }

    public void delete() {
        System.out.println("Đã xóa nhóm: " + groupName);
    }
}

class ContactGroup {
    private int contactId;
    private int groupId;

    public ContactGroup(int contactId, int groupId) {
        this.contactId = contactId;
        this.groupId = groupId;
    }

    public int getContactId() { return contactId; }
    public void setContactId(int contactId) { this.contactId = contactId; }

    public int getGroupId() { return groupId; }
    public void setGroupId(int groupId) { this.groupId = groupId; }

    public void assign() {
        System.out.println("Gán Contact ID " + contactId + " vào Group ID " + groupId);
    }

    public void remove() {
        System.out.println("Gỡ Contact ID " + contactId + " khỏi Group ID " + groupId);
    }
}

class PhoneBook {
    private int bookId;
    private List<Contact> contacts;
    private List<Group> groups;
    private List<ContactGroup> contactGroups;

    public PhoneBook(int bookId) {
        this.bookId = bookId;
        this.contacts = new ArrayList<>();
        this.groups = new ArrayList<>();
        this.contactGroups = new ArrayList<>();
    }

    public int getBookId() { return bookId; }
    public List<Contact> getContacts() { return contacts; }
    public List<Group> getGroups() { return groups; }

    public void addContact(Contact contact) {
        contacts.add(contact);
        System.out.println("Đã thêm liên hệ: " + contact.getFullName());
    }

    public List<Contact> searchContact(String keyword) {
        List<Contact> results = new ArrayList<>();
        for (Contact c : contacts) {
            if (c.getFullName().toLowerCase().contains(keyword.toLowerCase())) {
                results.add(c);
            }
        }
        return results;
    }

    public void updateContact(Contact updatedContact) {
        for (int i = 0; i < contacts.size(); i++) {
            if (contacts.get(i).getContactId() == updatedContact.getContactId()) {
                contacts.set(i, updatedContact);
                System.out.println("Đã cập nhật liên hệ ID: " + updatedContact.getContactId());
                return;
            }
        }
    }

    public void deleteContact(int contactId) {
        contactGroups.removeIf(cg -> cg.getContactId() == contactId);
        contacts.removeIf(c -> c.getContactId() == contactId);
        System.out.println("Đã xóa liên hệ ID: " + contactId);
    }

    public void addGroup(Group group) {
        groups.add(group);
        System.out.println("Đã thêm nhóm: " + group.getGroupName());
    }

    public void importContact() {
        System.out.println("Đang nhập danh bạ từ tệp...");
    }

    public void exportContact() {
        System.out.println("Đang xuất danh bạ ra tệp...");
    }
}

public class Main {
    public static void main(String[] args) {
        PhoneBook myPhoneBook = new PhoneBook(1);

        Contact c1 = new Contact(101, "Nguyen Van A", "a.nguyen@email.com", "Hà Nội", "Bạn thân", true);
        
        PhoneNumber p1 = new PhoneNumber(1, 101, "0912345678", "Mobile");
        c1.addPhoneNumber(p1);

        myPhoneBook.addContact(c1);

        Group g1 = new Group(1, "Gia đình", "Nhóm người thân trong gia đình");
        myPhoneBook.addGroup(g1);

        ContactGroup cg1 = new ContactGroup(c1.getContactId(), g1.getGroupId());
        cg1.assign();

        System.out.println("\n--- Kết quả tìm kiếm 'Nguyen' ---");
        List<Contact> searchResults = myPhoneBook.searchContact("Nguyen");
        for (Contact c : searchResults) {
            System.out.println("- " + c.getFullName() + " (" + c.getEmail() + ")");
        }
    }
}
