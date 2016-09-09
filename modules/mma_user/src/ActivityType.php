<?php

namespace Drupal\mma_user;

use Drupal\Core\StringTranslation\StringTranslationTrait;

class ActivityType {

  // Allows the use of t() from this class.
  use StringTranslationTrait;

  protected $definition;

  /**
   * ActivityType constructor.
   *
   * @param array $definition
   *   The activity type information.
   */
  public function __construct($definition = []) {
    $this->definition = $definition;
  }

  /**
   * Get the Marketo activity ID.
   *
   * @return string
   *   The Activity ID.
   */
  public function id() {
    return $this->definition['id'];
  }

  /**
   * Gets the name for a Marketo MA activity type.
   *
   * @return string
   *   The activity type name.
   */
  public function getName() {
    return $this->definition['name'];
  }

  /**
   * Gets the description for a Marketo MA activity type.
   *
   * @return string
   *   The activity type description.
   */
  public function getDescription() {
    return $this->definition['description'];
  }

  /**
   * Gets the name of the primary attribute for the activity type.
   *
   * @return string
   *   The name of the primary attributes.
   */
  public function getPrimaryAttributeName() {
    return $this->definition['primaryAttribute']['name'];
  }


  /**
   * Convert sthis activity type to a tableselect option.
   *
   * @return array
   *   This activity type converted to a tableselect option.
   */
  public function toTableSelectOption() {
    return [
      $this->t(':value', [':value' => $this->id()]),
      $this->t(':value', [':value' => $this->getName()]),
      $this->t(':value', [':value' => $this->getDescription()]),
      $this->t(':value', [':value' => $this->getPrimaryAttributeName()]),
    ];
  }
}
