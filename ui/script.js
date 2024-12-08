document.addEventListener('DOMContentLoaded', function() {
  // ارسال درخواست GET به API برای دریافت لیست تسک‌ها
  async function fetchTasks() {
      try {
          const response = await fetch('/tasks');
          if (response.ok) {
              const tasks = await response.json();
              displayTasks(tasks); // نمایش تسک‌ها در صفحه
          } else {
              console.error('Failed to fetch tasks');
          }
      } catch (error) {
          console.error('Error fetching tasks:', error);
      }
  }

  // نمایش تسک‌ها در صفحه
  function displayTasks(tasks) {
      const taskList = document.getElementById('task-list');
      const completedTaskList = document.getElementById('completed-task-list');
      taskList.innerHTML = ''; // پاک کردن لیست تسک‌های فعال
      completedTaskList.innerHTML = ''; // پاک کردن لیست تسک‌های تمام شده

      tasks.forEach((task, index) => {
          const taskItem = document.createElement('li');
          taskItem.className = 'task-item';
          
          const taskText = document.createElement('span');
          taskText.textContent = task.name;
          if (task.done) {
              taskItem.classList.add('completed'); // اضافه کردن کلاس برای تسک تمام شده
          }

          const completeButton = document.createElement('button');
          completeButton.textContent = 'Complete';
          completeButton.addEventListener('click', () => toggleTaskCompletion(index, task));

          const deleteButton = document.createElement('button');
          deleteButton.textContent = 'Delete';
          deleteButton.addEventListener('click', () => deleteTask(task._id));

          const editButton = document.createElement('button');
          editButton.textContent = 'Edit';
          editButton.addEventListener('click', () => editTask(task));

          const taskActions = document.createElement('div');
          taskActions.className = 'task-actions';
          taskActions.appendChild(completeButton);
          taskActions.appendChild(editButton);
          taskActions.appendChild(deleteButton);

          taskItem.appendChild(taskText);
          taskItem.appendChild(taskActions);

          // اضافه کردن تسک به لیست مربوطه
          if (task.done) {
              completedTaskList.appendChild(taskItem); // اضافه کردن به لیست تمام شده‌ها
          } else {
              taskList.appendChild(taskItem); // اضافه کردن به لیست تسک‌های فعال
          }
      });
  }

  // ارسال درخواست POST برای اضافه کردن تسک جدید
  async function addTask(taskText) {
      try {
          const response = await fetch('/add', {
              method: 'POST',
              headers: {
                  'Content-Type': 'application/json',
              },
              body: JSON.stringify({ name: taskText, done: false }), // اضافه کردن وضعیت done
          });

          if (response.ok) {
              fetchTasks(); // بازخوانی لیست تسک‌ها بعد از اضافه کردن تسک جدید
          } else {
              console.error('Failed to add task');
          }
      } catch (error) {
          console.error('Error adding task:', error);
      }
  }

  // ارسال درخواست برای تغییر وضعیت تکمیل شدن تسک
  async function toggleTaskCompletion(index, task) {
      try {
          task.done = !task.done; // تغییر وضعیت تکمیل
          const response = await fetch(`/update`, {
              method: 'POST',
              headers: {
                  'Content-Type': 'application/json',
              },
              body: JSON.stringify(task),
          });

          if (response.ok) {
              fetchTasks(); // بازخوانی لیست تسک‌ها پس از تغییر وضعیت
          } else {
              console.error('Failed to update task completion');
          }
      } catch (error) {
          console.error('Error toggling task completion:', error);
      }
  }

  // ارسال درخواست برای حذف تسک
  async function deleteTask(id) {
      try {
          const response = await fetch(`/task/${id}`, {
              method: 'DELETE',
          });

          if (response.ok) {
              fetchTasks(); // بازخوانی لیست تسک‌ها پس از حذف تسک
          } else {
              console.error('Failed to delete task');
          }
      } catch (error) {
          console.error('Error deleting task:', error);
      }
  }

  // ارسال درخواست برای ویرایش تسک
  async function editTask(task) {
      const newText = prompt('Enter new text for the task:');
      if (newText) {
          task.name = newText;
          try {
              const response = await fetch(`/update`, {
                  method: 'POST',
                  headers: {
                      'Content-Type': 'application/json',
                  },
                  body: JSON.stringify(task),
              });

              if (response.ok) {
                  fetchTasks(); // بازخوانی لیست تسک‌ها پس از ویرایش تسک
              } else {
                  console.error('Failed to edit task');
              }
          } catch (error) {
              console.error('Error editing task:', error);
          }
      }
  }

  // ارسال درخواست برای اضافه کردن تسک جدید از طریق فرم
  const taskForm = document.querySelector('form');
  if (taskForm) {
      taskForm.addEventListener('submit', (e) => {
          e.preventDefault();
          const taskText = document.getElementById('new-task').value;
          if (taskText) {
              addTask(taskText); // اضافه کردن تسک جدید
          }
      });
  }

  // فراخوانی تابع برای بارگذاری اولیه تسک‌ها
  fetchTasks();
});
