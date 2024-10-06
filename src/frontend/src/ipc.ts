type Id = number;

declare namespace __KRITAJS__ {
  function getClass(name: string): Id;
  function invoke(instanceId: Id, methodName: string, args?: unknown[]): unknown;
  function free(instanceId: Id): void;
}

export const getClass = __KRITAJS__.getClass;
export const invoke = __KRITAJS__.invoke;
export const free = __KRITAJS__.free;

class Krita {
  __id__: Id;

  private constructor(id: Id) {
    this.__id__ = id;
  }

  version() {
    const result = invoke(this.__id__, "version");
    return result;
  }

  static instance() {
    const kritaClass = getClass("Krita");
    const instanceId = invoke(kritaClass, "instance") as Id;
    return new Krita(instanceId);
  }
  
  free() {
    free(this.__id__);
  }
}

const krita = Krita.instance();
