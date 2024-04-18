#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;


// Structure to represent a to-do item
struct TodoItem {
    string task;
    bool completed;
    bool selected; // Flag to indicate selection
    TodoItem(const string& t, bool c = false) : task(t), completed(c), selected(false) {}
};


// Class for managing the to-do list
class TodoList {
private:
    const int taskSpacing = 75; // Spacing between tasks
    const int checkbox = 2; // Thickness of the checkbox
    const int taskStartY = 350; // Initial y position for tasks


public:
    vector<TodoItem> items;
    // Add a new item to the to-do list
    void addItem(const string& task) {
        items.emplace_back(task);
    }

    // Remove an item from the to-do list
    void removeItem(int index) {
        if (index >= 0 && index < items.size()) {
            items.erase(items.begin() + index);
        }
    }

    // Toggle selection of a task
    void toggleSelection(int index) {
        if (index >= 0 && index < items.size()) {
            items[index].selected = !items[index].selected;
        }
    }

    // Toggle completion status of a task
    void toggleCompletion(int index) {
        if (index >= 0 && index < items.size()) {
            items[index].completed = !items[index].completed;
        }
    }

    // Draw the to-do list on the given image
    void drawList(Mat& img) {
        // Draw instructions
        putText(img, "Press 'a' to add a task", Point(100, 100), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 3); // Black text, thicker font
        putText(img, "Press 'c' to complete selected tasks", Point(100, 150), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 3); // Black text, thicker font
        putText(img, "Press 'd' to delete selected tasks", Point(100, 200), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 3); // Black text, thicker font

        // Draw tasks
        int yPos = taskStartY;
        for (size_t i = 0; i < items.size(); ++i) {
            Scalar textColor = Scalar(0, 0, 0); // black text
            Scalar selectionColor = items[i].selected ? Scalar(255, 0, 0) : Scalar(209, 249, 254); // blue if selected, yellow if not selected

            // Draw selection box
            rectangle(img, Point(90, yPos - 30), Point(50, yPos + 10), selectionColor, FILLED);
            rectangle(img, Point(90, yPos - 30), Point(50, yPos + 10), Scalar(0, 0, 0), checkbox); // checkbox

            // Draw completed indicator box if task is completed
            if (items[i].completed) {
                rectangle(img, Point(80, yPos - 20), Point(60, yPos + 0), Scalar(0, 0, 255), FILLED);
            } // Red box // Smaller # pt1(right, up), pt2(left, down)

            // Draw task text
            putText(img, items[i].task, Point(110, yPos), FONT_HERSHEY_PLAIN, 3, textColor, 2); // thinner text
            yPos += taskSpacing; // Increment y position for next task
        }
    }

    // Check if a point is within the bounding box of a task
    int findTaskIndex(const Point& point) {
        int index = -1;
        for (size_t i = 0; i < items.size(); ++i) {
            int y_min = taskStartY + i * taskSpacing - 30;
            int y_max = taskStartY + i * taskSpacing + 10;
            if (point.y >= y_min && point.y <= y_max && point.x >= 30 && point.x <= 90) {
                index = i;
                break;
            }
        }
        return index;
    }

    // Get index of multiple selected tasks
    vector<int> getSelectedTaskIndices() {
        vector<int> selectedIndices;
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].selected) {
                selectedIndices.push_back(i);
            }
        }
        return selectedIndices;
    }


    // Delete selected tasks and move remaining tasks up
    void deleteSelectedTasks() {
        vector<int> selectedIndices = getSelectedTaskIndices();
        if (!selectedIndices.empty()) {
            for (int i = selectedIndices.size() - 1; i >= 0; --i) {
                removeItem(selectedIndices[i]);
            }
        }
    }

    // Save the task list to a text file
    void saveToFile(const string& filename) {
        ofstream outFile(filename);
        if (outFile.is_open()) {
            for (const auto& item : items) {
                if (item.completed) {
                    outFile << "[x] " << item.task << endl;
                }
                else {
                    outFile << "[ ] " << item.task << endl;
                }
            }
            outFile.close();
        }
    }
};


int main() {
    // Add the entire instruction list to the output terminal
    cout << "Click on a task to select" << endl
         << "Press 'a' to add a task" << endl
         << "Press 'c' to complete selected tasks" << endl
         << "Press 'd' to delete selected tasks" << endl
         << "Press 'q' to quit program and to title task list" << endl;


    // Create an empty to-do list
    TodoList todoList;
    // Create a window to display the to-do list
    namedWindow("To-Do List", WINDOW_NORMAL); // Making the window resizable
    resizeWindow("To-Do List", 800, 600); // Setting initial window size // (x, y)
    Mat canvas(1050, 1680, CV_8UC3);
    string inputText; // store text input
    while (true) {
        // Clear the canvas with yellow background
        canvas = Scalar(209, 249, 254); // (notepad yellow) B: 209 G: 249 R: 254
        // Draw the to-do list on the canvas
        todoList.drawList(canvas);
        // Display the canvas
        imshow("To-Do List", canvas);
        // Wait for key press
        int key = waitKey(10);
        // If a key is pressed
        if (key != -1) {
            // If 'a' is pressed, add a new task
            if (key == 'a' || key == 'A') {
                // Prompt user to enter a new task
                putText(canvas, "Enter new task:", Point(100, 250), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 3);
                imshow("To-Do List", canvas);
                inputText.clear(); // Clear text input
                while (true) {
                    int charCode = waitKey(0);
                    if (charCode == 13) { // Enter key
                        todoList.addItem(inputText);
                        break;
                    }
                    else if (charCode == 27) { // Escape key
                        break;
                    }
                    else if (charCode == 8) { // Backspace key (ASCII 8)
                        if (!inputText.empty()) {
                            inputText.pop_back();
                            // Clear the area where input text is displayed
                            rectangle(canvas, Point(100, 300 - 30), Point(800, 300 + 10), Scalar(209, 249, 254), FILLED); // Fill with background color
                            imshow("To-Do List", canvas); // Update display
                        }
                    }
                    else {
                        inputText += static_cast<char>(charCode);
                    }
                    putText(canvas, inputText, Point(100, 300), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 2); // Black text, thinner font
                    imshow("To-Do List", canvas);
                }
            }
                // If 'c' is pressed, toggle completion status of selected tasks
            else if (key == 'c' || key == 'C') {
                vector<int> selectedIndices = todoList.getSelectedTaskIndices();
                for (int index : selectedIndices) {
                    todoList.toggleCompletion(index);
                    todoList.items[index].selected = false; // Deselect task after toggling completion
                }
            }
                // If 'd' is pressed, delete selected tasks
            else if (key == 'd' || key == 'D') {
                todoList.deleteSelectedTasks();
            }
                // If 'q' is pressed, quit the application
            else if (key == 'q' || key == 'Q') {
                // Clear instructions and exit loop
                rectangle(canvas, Point(0, 0), Point(1050, 250), Scalar(209, 249, 254), FILLED); // Fill with background color
                imshow("To-Do List", canvas); // Update display
                // Save the task list to a text file
                putText(canvas, "Enter a title for the task list:", Point(100, 50), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 3); // Black text, thicker font
                imshow("To-Do List", canvas);
                inputText.clear(); // Clear text input
                while (true) {
                    int charCode = waitKey(0);
                    if (charCode == 13) { // Enter key
                        // Save the task list to a text file
                        todoList.saveToFile(inputText + ".txt");
                        break;
                    }
                    else if (charCode == 27) { // Escape key
                        break;
                    }
                    else if (charCode == 8) { // Backspace key (ASCII 8)
                        if (!inputText.empty()) {
                            inputText.pop_back();
                            // Clear the area where input text is displayed
                            rectangle(canvas, Point(100, 100 - 30), Point(800, 100 + 10), Scalar(209, 249, 254), FILLED); // Fill with background color
                            imshow("To-Do List", canvas); // Update display
                        }
                    }
                    else {
                        inputText += static_cast<char>(charCode);
                    }
                    putText(canvas, inputText, Point(100, 100), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 2); // Black text, thinner font
                    imshow("To-Do List", canvas);
                }
                // Exit the program
                break;
            }
        }
        // If a mouse event occurs
        if (key == -1) {
            setMouseCallback("To-Do List", [](int event, int x, int y, int flags, void* userdata) {
                if (event == EVENT_LBUTTONDOWN) {
                    TodoList* todoList = (TodoList*)userdata;
                    Point mousePosition(x, y);
                    int taskIndex = todoList->findTaskIndex(mousePosition);
                    if (taskIndex != -1) {
                        todoList->toggleSelection(taskIndex);
                    }
                }
            }, (void*)&todoList);
        }
    }


    // Destroy the window
    cv::destroyWindow("To-Do List");
    return 0;
}
