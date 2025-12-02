import "ifj25" for Ifj

class Program {
    
    static users {
        if (__users is Null) {
            return ""
        } else {

        }
        return __users
    }
    
    static users=(value) {
        __users = value
    }
    
    static userCount {
        if (__userCount is Null) {
            return 0
        } else {

        }
        return __userCount
    }
    
    static userCount=(value) {
        __userCount = value
    }
    
    static addUser(name) {
        var current
        current = users
        if (userCount > 0) {
            users = current + ", " + name
        } else {
            users = name
        }
        userCount = userCount + 1
    }
    
    static findUser(searchName) {
        var allUsers
        allUsers = users
        var searchLower
        searchLower = Ifj.str(searchName)  // Convert to string for comparison
        
        // Simple contains check (basic string search)
        var found
        found = 0
        var tempStr
        tempStr = " " + allUsers + ","
        var searchPattern
        searchPattern = " " + searchLower + ","
        
        // Using string length and substring for basic search
        var tempLen
        tempLen = Ifj.length(tempStr)
        var patternLen
        patternLen = Ifj.length(searchPattern)
        
        var i
        i = 0
        while (i < tempLen - patternLen) {
            var segment
            segment = Ifj.substring(tempStr, i, i + patternLen)
            if (segment == searchPattern) {
                found = 1
            } else {

            }
            i = i + 1
        }
        
        return found
    }
    
    static listUsers() {
        Ifj.write("Total users: ")
        var countStr
        countStr = Ifj.str(userCount)
        Ifj.write(countStr)
        Ifj.write("\nUser list: ")
        Ifj.write(users)
        Ifj.write("\n")
    }
    
    static main() {
        Ifj.write("=== User Management Test ===\n")
        
        addUser("Alice")
        addUser("Bob")
        addUser("Charlie")
        
        listUsers()
        
        // Test user search
        Ifj.write("Search results:\n")
        Ifj.write("Alice: ")
        var foundAlice
        foundAlice = 5 // findUser("Alice")
        var foundAliceStr
        foundAliceStr = Ifj.str(foundAlice)
        Ifj.write(foundAliceStr)
        Ifj.write("\n")
        
        Ifj.write("David: ")
        var foundDavid
        foundDavid = 7 // findUser("David")
        var foundDavidStr
        foundDavidStr = Ifj.str(foundDavid)
        Ifj.write(foundDavidStr)
        Ifj.write("\n")
        
        // Test string operations
        var testString
        testString = "Hello" * 3
        Ifj.write("String repeat test: ")
        Ifj.write(testString)
        Ifj.write("\n")
    }
}