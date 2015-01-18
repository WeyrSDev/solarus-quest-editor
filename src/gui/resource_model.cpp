/*
 * Copyright (C) 2014-2015 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "gui/resource_model.h"
#include "quest.h"
#include "quest_resources.h"

/**
 * @brief Creates a resource model.
 * @param quest The quest.
 * @param resource_type Type of resources to show.
 * @param parent The parent object or nullptr.
 */
ResourceModel::ResourceModel(Quest& quest, ResourceType resource_type, QObject* parent) :
  QStandardItemModel(parent),
  quest(quest),
  resource_type(resource_type) {

  QStringList ids = get_resources().get_elements(this->resource_type);
  for (const QString& id : ids) {
    add_element(id);
  }

  // TODO update when resources change.
}

/**
 * @brief Returns the quest.
 * @return The quest.
 */
Quest& ResourceModel::get_quest() {
  return quest;
}

/**
 * @brief Returns the resources of the quest.
 * @return The resources.
 */
QuestResources& ResourceModel::get_resources() {
  return quest.get_resources();
}

/**
 * @brief Returns the index of the specified element.
 * @param element_id Id of a resource element.
 * @return The corresponding index in this model or -1 if it does not exist.
 */
int ResourceModel::get_element_index(const QString& element_id) const {

  const QStandardItem* item = get_element_item(element_id);
  return indexFromItem(item).row();
}

/**
 * @brief Adds to the model an item for the specified resource element.
 * @param element_id Id of the resource element to add.
 */
void ResourceModel::add_element(const QString& element_id) {

  QStringList files = element_id.split('/', QString::SkipEmptyParts);
  QStandardItem* parent = invisibleRootItem();
  while (files.size() > 1) {
    parent = find_or_create_dir_item(*parent, files.first());
    files.removeFirst();
  }

  QStandardItem* item = create_element_item(element_id);
  parent->appendRow(item);
}

/**
 * @brief Creates a new leaf item with the specified element id.
 * @param element_id Id of the element to create.
 * @return The created item.
 */
QStandardItem* ResourceModel::create_element_item(const QString& element_id) {

  QString description = get_resources().get_description(resource_type, element_id);

  QStandardItem* item = new QStandardItem(description);
  QString resource_type_name = QString::fromStdString(
        Solarus::QuestResources::get_resource_type_name(resource_type));
  item->setData(
        QIcon(":/images/icon_resource_" + resource_type_name + ".png"),
        Qt::DecorationRole);
  item->setData(element_id, Qt::UserRole);
  items.insert(std::make_pair(element_id, item));
  return item;
}

/**
 * @brief Returns the leaf item with the specified element id.
 * @param element_id Id of the element to get.
 * @return The item or nullptr if it does not exist.
 */
const QStandardItem* ResourceModel::get_element_item(const QString& element_id) const {

  const auto& it = items.find(element_id);
  if (it == items.end()) {
    return nullptr;
  }

  return it->second;
}

/**
 * @brief Returns the leaf item with the specified element id.
 *
 * Non-const version.
 *
 * @param element_id Id of the element to get.
 * @return The item or nullptr if it does not exist.
 */
QStandardItem* ResourceModel::get_element_item(const QString& element_id) {

  const auto& it = items.find(element_id);
  if (it == items.end()) {
    return nullptr;
  }

  return it->second;
}

/**
 * @brief Returns the item with the specified directory name.
 * @param parent The parent item.
 * @param dir_name Name of the subdirectory to get.
 * @return The child. It is created if it does not exist yet.
 */
QStandardItem* ResourceModel::find_or_create_dir_item(
    QStandardItem& parent, const QString& dir_name) {

  for (int i = 0; i < parent.rowCount(); ++i) {
    QStandardItem* child = parent.child(i, 0);
    QString name = child->data(Qt::DisplayRole).toString();
    if (name == dir_name) {
      return child;
    }

    if (name > dir_name) {
      child = create_dir_item(dir_name);
      parent.insertRow(i, child);
      return child;
    }
  }

  QStandardItem* child = create_dir_item(dir_name);
  parent.appendRow(child);
  return child;
}

/**
 * @brief Creates a new item with the specified directory name.
 * @param dir_name Name of a directory.
 * @return The created item.
 */
QStandardItem* ResourceModel::create_dir_item(const QString& dir_name) {

  QStandardItem* item = new QStandardItem(dir_name);
  item->setSelectable(false);
  item->setData(QIcon(":/images/icon_folder_open.png"), Qt::DecorationRole);
  return item;
}
