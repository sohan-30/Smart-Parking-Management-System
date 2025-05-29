# Smart Parking Management System

## Description
A structured and optimized C-based application that manages parking lots using self-balancing AVL Trees. The system stores and organizes vehicle and parking slot data for efficient searching, insertion, and updates, even under heavy load. It provides intelligent vehicle allocation based on user membership, current occupancy, and slot availability. Multiple AVL trees are maintained for various sorted views like revenue, occupancy, and usage frequency, ensuring fast operations and data consistency.

## Features

- **Vehicle Registration with Validation**
  - Each vehicle is registered using a unique vehicle number (VN), along with the user’s name, membership type, parking slot ID, total hours parked, total parkings made, and total amount paid.
  - Duplicate vehicle numbers are prevented through AVL-based checks.

- **AVL Tree-based Data Storage**
  - AVL Trees are used to maintain balance in multiple data structures:
    - By Vehicle Number (VN)
    - By Total Amount Paid
    - By Total Parking Count
    - By Occupancy Count
    - By Revenue per Slot
  - Ensures `O(log n)` complexity for insertions, deletions, and searches.

- **Intelligent Tree Rotations**
  - Automatically handles LL, RR, LR, and RL rotations during insertion to maintain tree balance.
  - Fully custom balancing logic implemented with detailed balance factor tracking.

- **Real-time Parking Slot Management**
  - Tracks current occupancy and availability of each slot.
  - Updates occupancy count and revenue in real-time as users park.

- **Membership-based Smart Allocation**
  - Allocates parking based on membership types (e.g., regular, premium or golden).
  - Searches for the first eligible, unoccupied slot based on a prioritized in-order traversal.

- **Dynamic Sorting and Views**
  - Different AVL trees allow sorted views by:
    - Total amount paid
    - Number of parkings made
    - Parking slot occupancy
    - Revenue generated
  - Enables performance analysis and user behavior tracking.

- **Flexible Search and Traversals**
  - Implements multiple traversal methods: in-order, pre-order, level-order (BFS).
  - Allows filtering and viewing user details or slot status in various orders.

- **Modular and Scalable Design**
  - Core logic written in clean, modular C functions.
  - Can be extended easily for billing, reporting, or UI integration.

##  Technologies & Tools

- **Language:**  
  - C  
    - Core application logic written in structured and modular C.
    - Manual memory management using pointers and dynamic allocation (`malloc`/`free`).
    - Emphasis on performance and data structure implementation.

- **Data Structures:**  
  - AVL Trees  
    - Custom-built self-balancing binary search trees to manage parking data.
    - Implemented rotation logic (LL, RR, LR, RL) from scratch.
    - Maintained separate AVL trees for multiple sorting criteria.

- **Compiler:**  
  - GCC (GNU Compiler Collection)

- **Development Environment:**  
  - Linux/Unix or Windows terminal
  - Code Editor: VS Code / Sublime / Code::Blocks

- **Version Control:**  
  - Git & GitHub  
    - Tracks changes and version history.
    - Enables collaboration and showcases project evolution.

## How It Works

### Initialization

- At program start, two main AVL trees are initialized as `NULL`:
  - `details* detailsRoot;` — stores registered vehicle details.
  - `Status* statusRoot;` — stores parking slot occupancy and revenue data.
- Initial test data can optionally be loaded to simulate real-world usage.

### Core Components & Data Flow

#### 1. User Registration
- Users provide vehicle number (VN), owner name, and membership type.
- A new `details` node is created and inserted into the details AVL tree, sorted by `vn`.
- AVL rotations maintain tree balance after each insertion.

#### 2. Parking Allocation
- When a user requests parking, the system searches for the first eligible vacant `Status` node.
- The AVL tree is traversed in-order to find a slot matching membership eligibility.
- Once found, occupancy is incremented, status set to occupied, and user's parking slot ID updated.

#### 3. AVL Tree Insertions with Custom Keys
- Multiple insertion functions maintain AVL trees sorted by different keys:
  - `insertByAmount()` — sorts by total amount paid.
  - `insertByParkings()` — sorts by total parkings.
  - `insertByOccupancy()` — sorts parking slots by occupancy.
  - `insertByRevenue()` — sorts parking slots by revenue.
- All maintain balance using LL, RR, LR, and RL rotations.

#### 4. Deallocation & Revenue Update
- When a user leaves:
  - Occupancy of their parking slot decrements.
  - Total parking hours and amount paid are calculated.
  - Revenue in both `Status` and `details` nodes are updated accordingly.

### AVL Tree Balancing

- After each insertion, the balance factor (height difference between left and right subtrees) is checked.
- If the balance factor exceeds ±1, appropriate rotations (LL, RR, LR, RL) restore balance.
- Each node maintains a `height` field for balance calculations.

### Searching & Reporting

- Custom search functions allow:
  - Finding vehicles by VN.
  - Finding parking slots by ID.
- Reporting functions list users sorted by amount paid or parking frequency.
- Parking slots can be reported sorted by occupancy or revenue.

### Special Logic

- Equal key values during insertion (e.g., same amount paid) are always inserted into the right subtree for consistency.
- Membership eligibility is checked during parking allocation for priority handling.
- Memory management is carefully handled with dynamic allocation and balanced AVL tree updates.

## Compilation and Execution

**On Windows, macOS, and Linux:**

1. **Open a terminal or command prompt**
2. **Navigate to the directory containing `SmartCarParkingSystemAVL.c`**
3. **Compile and run the code:**

   * **On Windows:**
   ```
   gcc SmartCarParkingSystemAVL.c -o SmartCarParkingSystemAVL.exe
   SmartCarParkingSystemAVL.exe
   ```

   * **On macOS/Linux:**
   ```
   gcc SmartCarParkingSystemAVL.c -o SmartCarParkingSystemAVL
   ./SmartCarParkingSystemAVL
   ```
