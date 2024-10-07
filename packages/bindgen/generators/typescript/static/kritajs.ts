export type Id = number;

declare namespace __KRITAJS__ {
  function invokeStatic<T>(className: string, methodName: string, args?: unknown[]): T;
  function invoke<T>(instanceId: Id, methodName: string, args?: unknown[]): T;
  function free(instanceId: Id): void;
}

export const invokeStatic = __KRITAJS__.invokeStatic;
export const invoke = __KRITAJS__.invoke;
export const free = __KRITAJS__.free;
