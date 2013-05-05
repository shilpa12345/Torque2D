//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

ConsoleMethodGroupBeginWithDocs(SimObject, ConsoleObject)

/*! returns the objects internal name
*/
ConsoleMethodWithDocs( SimObject, getInternalName, ConsoleString, 2, 2, ())
{
   return object->getInternalName();
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, dumpClassHierarchy, ConsoleVoid, 2, 2, ())
{
   object->dumpClassHierarchy();
}

/*! returns true if this object is a member of the specified class
*/
ConsoleMethodWithDocs(SimObject, isMemberOfClass, ConsoleBool, 3, 3,  (string classname))
{

   AbstractClassRep* pRep = object->getClassRep();
   while(pRep)
   {
      if(!dStricmp(pRep->getClassName(), argv[2]))
      {
         //matches
         return true;
      }

      pRep	=	pRep->getParentClass();
   }

   return false;
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, save, ConsoleBool, 3, 4, (fileName, [selectedOnly]?))
{
   bool bSelectedOnly	=	false;
   if(argc > 3)
      bSelectedOnly	= dAtob(argv[3]);

   const char* filename = NULL;

   filename = argv[2];

   if(filename == NULL || *filename == 0)
      return false;

   return object->save(filename, bSelectedOnly);

}

/*! 
*/
ConsoleMethodWithDocs(SimObject, setName, ConsoleVoid, 3, 3, (newName))
{
   object->assignName(argv[2]);
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getName, const char *, 2, 2, ())
{
   const char *ret = object->getName();
   return ret ? ret : "";
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getClassName, const char *, 2, 2, ())
{
   const char *ret = object->getClassName();
   return ret ? ret : "";
}

/*!
*/
ConsoleMethodWithDocs(SimObject, getFieldValue, const char *, 3, 3, (fieldName))
{
   const char *fieldName = StringTable->insert( argv[2] );
   return object->getDataField( fieldName, NULL );
}


/*!
*/
ConsoleMethodWithDocs(SimObject, setFieldValue, ConsoleBool, 4, 4, (fieldName,value))
{
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];

   object->setDataField( fieldName, NULL, value );

   return true;

}

/*!
*/
ConsoleMethodWithDocs(SimObject, getFieldType, const char *, 3, 3, (fieldName))
{
   const char *fieldName = StringTable->insert( argv[2] );
   U32 typeID = object->getDataFieldType( fieldName, NULL );
   ConsoleBaseType* type = ConsoleBaseType::getType( typeID );
   
   if( type )
      return type->getTypeClassName();

   return "";
}

/*! Dynamically call a method on an object.
*/
ConsoleMethodWithDocs( SimObject, call, ConsoleString, 2, 0, ( functionName, [args]* ))
{
   argv[1] = argv[2];
   return Con::execute( object, argc - 1, argv + 1 );
}

//-----------------------------------------------------------------------------
//	Set the internal name, can be used to find child objects
//	in a meaningful way, usually from script, while keeping
//	common script functionality together using the controls "Name" field.
//-----------------------------------------------------------------------------
/*! 
*/
ConsoleMethodWithDocs( SimObject, setInternalName, ConsoleVoid, 3, 3, (string InternalName))
{
   object->setInternalName(argv[2]);
}

/*! Use the getId method to get the numeric ID of this shape.
    @return Returns the unique numeric ID of this shape.
    @sa getName, setName
*/
ConsoleMethodWithDocs(SimObject, getId, ConsoleInt, 2, 2, ())
{
   return object->getId();
}

/*! Use the getGroup method to determine if this object is contained in a SimGroup and if so, which one.
    @return Returns the ID of the SimGroup this shape is in or zero if the shape is not contained in a SimGroup
*/
ConsoleMethodWithDocs(SimObject, getGroup, ConsoleInt, 2, 2, ())
{
   SimGroup *grp = object->getGroup();
   if(!grp)
      return -1;
   return grp->getId();
}

/*! Use the delete method to delete this object.
    When an object is deleted, it automatically: Unregisters its ID and name (if it has one) with the engine.
	Removes itself from any SimGroup or SimSet it may be a member of. (eventually) returns the memory associated with
	itself and its non-dynamic members. Cancels all pending %obj.schedule() events.
	For objects in the GameBase, ScriptObject, or GUIControl hierarchies, an object will first: Call the onRemove() method for the object's namespace
    @return No return value.
*/
ConsoleMethodWithDocs(SimObject, delete, ConsoleVoid, 2, 2, ())
{
   object->deleteObject();
}

/*! Use the schedule method to schedule an action to be executed upon this object time milliseconds in the future.
    The major difference between this and the schedule console function is that if this object is deleted prior
	to the scheduled event, the event is automatically canceled. times should not be treated as exact since some
	'simulation delay' is to be expected. The minimum resolution for a scheduled event is \32 ms, or one tick.
	schedule does not validate the existence of command. i.e. If you pass an invalid console method name, the
	schedule() method will still return a schedule ID, but the subsequent event will fail silently.
    @param time Time in milliseconds till action is scheduled to occur.
    @param command Name of the command to execute. This command must be scoped to this object (i.e. It must exist in the namespace of the object), otherwise the schedule call will fail.
    @param arg1...argN These are optional arguments which will be passed to command. This version of schedule automatically passes the ID of %obj as arg0 to command.
    @return Returns an integer schedule ID.
    @sa See the schedule console function and its corresponding helper functions.
*/
ConsoleMethodWithDocs(SimObject,schedule, ConsoleInt, 4, 0, (time , command , [arg]* ))
{
   U32 timeDelta = U32(dAtof(argv[2]));
   argv[2] = argv[3];
   argv[3] = argv[1];
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 2, argv + 2, true);
   S32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + timeDelta);
// #ifdef DEBUG
//    Con::printf("obj %s schedule(%s) = %d", argv[3], argv[2], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

static S32 QSORT_CALLBACK compareFields(const void* a,const void* b)
{
   const AbstractClassRep::Field* fa = *((const AbstractClassRep::Field**)a);
   const AbstractClassRep::Field* fb = *((const AbstractClassRep::Field**)b);

   return dStricmp(fa->pFieldname, fb->pFieldname);
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getDynamicFieldCount, ConsoleInt, 2, 2, ())
{
   S32 count = 0;
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
      count++;

   return count;
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getDynamicField, ConsoleString, 3, 3, (index))
{
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   SimFieldDictionaryIterator itr(fieldDictionary);
   S32 index = dAtoi(argv[2]);
   for (S32 i = 0; i < index; i++)
   {
      if (!(*itr))
      {
         Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
         return NULL;
      }
      ++itr;
   }

   char* buffer = Con::getReturnBuffer(256);
   if (*itr)
   {
      SimFieldDictionary::Entry* entry = *itr;
      dSprintf(buffer, 256, "%s", entry->slotName);
      return buffer;
   }

   Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
   return NULL;
}

/*! Gets the number of persistent fields on the object.
*/
ConsoleMethodWithDocs( SimObject, getFieldCount, ConsoleInt, 2, 2, ())
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   const AbstractClassRep::Field* f;
   U32 numDummyEntries = 0;
   for(int i = 0; i < list.size(); i++)
   {
      f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType )
      {
         numDummyEntries++;
      }
   }

   return list.size() - numDummyEntries;
}

/*! Gets the name of the field at the given index.
*/
ConsoleMethodWithDocs( SimObject, getField, ConsoleString, 3, 3, (int index))
{
   S32 index = dAtoi( argv[2] );
   const AbstractClassRep::FieldList &list = object->getFieldList();
   if( ( index < 0 ) || ( index >= list.size() ) )
      return "";

   const AbstractClassRep::Field* f;
   S32 currentField = 0;
   for(int i = 0; i < list.size() && currentField <= index; i++)
   {
      f = &list[i];
      
      // skip any dummy fields   
      if(f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType)
      {
         continue;
      }

      if(currentField == index)
         return f->pFieldname;
      
      currentField++;
   }  

   // if we found nada, return nada.
   return "";
}

/*! Use the dump method to display the following information about this object: All engine and script registered console methods (including parent methods) for this object, all Non-Dynamic Fields AND all Dynamic Fields
    @return No return value
*/
ConsoleMethodWithDocs(SimObject,dump, ConsoleVoid, 2, 2, ())
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   char expandedBuffer[4096];

   Con::printf("Member Fields:");
   Vector<const AbstractClassRep::Field *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < (U32)list.size(); i++)
      flist.push_back(&list[i]);

   dQsort(flist.address(),flist.size(),sizeof(AbstractClassRep::Field *),compareFields);

   for(Vector<const AbstractClassRep::Field *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      const AbstractClassRep::Field* f = *itr;
      if( f->type == AbstractClassRep::DepricatedFieldType ||
          f->type == AbstractClassRep::StartGroupFieldType ||
          f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         // [neo, 07/05/2007 - #3000]
         // Some objects use dummy vars and projected fields so make sure we call the get functions 
         //const char *val = Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag);                          
         const char *val = (*f->getDataFn)( object, Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag) );// + typeSizes[fld.type] * array1));

         if(!val /*|| !*val*/)
            continue;
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s[%d] = \"", f->pFieldname, j);
         expandEscape(expandedBuffer + dStrlen(expandedBuffer), val);
         Con::printf("%s\"", expandedBuffer);
      }
   }

   Con::printf("Tagged Fields:");
   if(object->getFieldDictionary())
      object->getFieldDictionary()->printFields(object);

   Con::printf("Methods:");
   Namespace *ns = object->getNamespace();
   Vector<Namespace::Entry *> vec(__FILE__, __LINE__);

   if(ns)
      ns->getEntryList(&vec);

   for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
      Con::printf("  %s() - %s", (*j)->mFunctionName, (*j)->mUsage ? (*j)->mUsage : "");

}



/*! Use the getType method to get the type for this object. This type is an integer value composed of bitmasks. For simplicity, these bitmasks are defined in the engine and exposed for our use as global variables.
    To simplify the writing of scripts, a set of globals has been provided containing the bit setting for each class corresponding to a particular type.
    @return Returns a bit mask containing one or more set bits.
    @sa getClassName
*/
ConsoleMethodWithDocs(SimObject, getType, ConsoleInt, 2, 2, ())
{
   return((S32)object->getType());
}

bool SimObject::isMethod( const char* methodName )
{
   if( !methodName || !methodName[0] )
      return false;

   StringTableEntry stname = StringTable->insert( methodName );

   if( getNamespace() )
      return ( getNamespace()->lookup( stname ) != NULL );

   return false;
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, isMethod, ConsoleBool, 3, 3, (string method name))
{
   return object->isMethod( argv[2] );
}

/*! returns true, if we are in the specified simgroup - or a subgroup thereof
*/
ConsoleMethodWithDocs(SimObject, isChildOfGroup, ConsoleBool, 3,3, ())
{
   SimGroup* pGroup = dynamic_cast<SimGroup*>(Sim::findObject(dAtoi(argv[2])));
   if(pGroup)
   {
      return object->isChildOfGroup(pGroup);
   }

   return false;
}

/*! Clones the object.
    @param copyDynamicFields Whether the dynamic fields should be copied to the cloned object or not.  Optional: Defaults to false.
    @return (newObjectID) The newly cloned object's id if successful, otherwise a 0.
*/
ConsoleMethodWithDocs(SimObject, clone, ConsoleInt, 2, 3, ([bool copyDynamicFields? = false]?))
{
    // Fetch copy dynamic fields flag.
    const bool copyDynamicFields = ( argc >= 3 ) ? dAtob( argv[2] ) : false;

    // Clone Object.
    SimObject* pClonedObject = object->clone( copyDynamicFields );

    // Finish if object was not cloned.
    if ( pClonedObject == NULL )
        return 0;

    return pClonedObject->getId();
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getClassNamespace, ConsoleString, 2, 2, ())
{
   return object->getClassNamespace();
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, getSuperClassNamespace, ConsoleString, 2, 2, ())
{
   return object->getSuperClassNamespace();
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, setClassNamespace, ConsoleVoid, 2, 3, ())
{
   object->setClassNamespace(argv[2]);
}

/*! 
*/
ConsoleMethodWithDocs(SimObject, setSuperClassNamespace, ConsoleVoid, 2, 3, ())
{
   object->setSuperClassNamespace(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Sets the progenitor file responsible for this instances creation.
    @param file The progenitor file responsible for this instances creation.
    @return No return value.
*/
ConsoleMethodWithDocs(SimObject, setProgenitorFile, ConsoleVoid, 3, 3, (file))
{
    object->setProgenitorFile( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the progenitor file responsible for this instances creation.
    @return The progenitor file responsible for this instances creation.
*/
ConsoleMethodWithDocs(SimObject, getProgenitorFile, ConsoleString, 2, 2, ())
{
    return object->getProgenitorFile();
}


//-----------------------------------------------------------------------------

/*! Starts a periodic timer for this object.
    Sets a timer on the object that, when it expires, will cause the object to execute the onTimer() callback.
    The timer event will continue to occur at regular intervals until setTimerOff() is called.
    @param callbackFunction The name of the callback function to call for each timer repetition.
    @param timePeriod The period of time (in milliseconds) between each callback.
    @param repeat The number of times the timer should repeat.  If not specified or zero then it will run infinitely
    @return No return Value.
*/
ConsoleMethodWithDocs(SimObject, startTimer, ConsoleBool, 4, 5, (callbackFunction, float timePeriod, [repeat]?))
{
    // Is the periodic timer running?
    if ( object->getPeriodicTimerID() != 0 )
    {
        // Yes, so cancel it.
        Sim::cancelEvent( object->getPeriodicTimerID() );

        // Reset Timer ID.
        object->setPeriodicTimerID( 0 );
    }

    // Fetch the callback function.
    StringTableEntry callbackFunction = StringTable->insert( argv[2] );

    // Does the function exist?
    if ( !object->isMethod( callbackFunction ) )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The callback function of '%s' does not exist.", callbackFunction );
        return false;
    }

    // Fetch the time period.
    const S32 timePeriod = dAtoi(argv[3]);

    // Is the time period valid?
    if ( timePeriod < 1 )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The time period of '%d' is invalid.", timePeriod );
        return false;
    }        

    // Fetch the repeat count.
    const S32 repeat = argc >= 5 ? dAtoi(argv[4]) : 0;

    // Create Timer Event.
    SimObjectTimerEvent* pEvent = new SimObjectTimerEvent( callbackFunction, (U32)timePeriod, (U32)repeat );

    // Post Event.
    object->setPeriodicTimerID( Sim::postEvent( object, pEvent, Sim::getCurrentTime() + timePeriod ) );

    return true;
}

//-----------------------------------------------------------------------------

/*! Stops the periodic timer for this object.
    @return No return Value.
*/
ConsoleMethodWithDocs(SimObject, stopTimer, ConsoleVoid, 2, 2, ())
{
    // Finish if the periodic timer isn't running.
    if ( object->getPeriodicTimerID() == 0 )
        return;

    // Cancel It.
    Sim::cancelEvent( object->getPeriodicTimerID() );

    // Reset Timer ID.
    object->setPeriodicTimerID( 0 );
}

//-----------------------------------------------------------------------------

/*! Checks whether the periodic timer is active for this object or not.
    @return Whether the periodic timer is active for this object or not.
*/
ConsoleMethodWithDocs(SimObject, isTimerActive, ConsoleBool, 2, 2, ())
{
    return object->isPeriodicTimerActive();
}

ConsoleMethodGroupEndWithDocs(SimObject)
